/*
 * echoserverts.c - A concurrent echo server using threads
 * and a message buffer.
 */
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>

#include <vector>

struct room;

//create a struct to represent a user
struct user{
  char nickname[16];          //nickname of a user
  int connection_fd;          //socket connection between server and user
  room* cur_room;             //the current room the user is in
};

//create a struct to represent a chat room
struct room{
  std::vector<user*> user_list;  //list of all users within this room
  char room_pass[16];           //add a room_pass
};


/* Simplifies calls to bind(), connect(), and accept() */
typedef struct sockaddr SA;

/* Max text line length */
#define MAXLINE 8192

/* Second argument to listen() */
#define LISTENQ 1024

//initialize a vector of rooms that will be used as chat rooms
std::vector<room*> room_list;

// We will use this as a simple circular buffer of incoming messages.
char message_buf[20][50];

// This is an index into the message buffer.
int msgi = 0;

// A lock for the message buffer.
pthread_mutex_t lock;

// Initialize the message buffer to empty strings.
void init_message_buf() {
  int i;
  for (i = 0; i < 20; i++) {
    strcpy(message_buf[i], "");
  }
}

// This function adds a message that was received to the message buffer.
// Notice the lock around the message buffer.
void add_message(char *buf) {
  pthread_mutex_lock(&lock);
  strncpy(message_buf[msgi % 20], buf, 50);
  int len = strlen(message_buf[msgi % 20]);
  message_buf[msgi % 20][len] = '\0';
  msgi++;
  pthread_mutex_unlock(&lock);
}

// A wrapper around send to simplify calls.
int send_message(int connfd, char *message) {
  return send(connfd, message, strlen(message), 0);
}

// A predicate function to test incoming message.
int is_list_message(char *message) { return strncmp(message, "-", 1) == 0; }

int send_list_message(int connfd) {
  char message[20 * 50] = "";
  for (int i = 0; i < 20; i++) {
    if (strcmp(message_buf[i], "") == 0) break;
    strcat(message, message_buf[i]);
    strcat(message, ",");
  }

  // End the message with a newline and empty. This will ensure that the
  // bytes are sent out on the wire. Otherwise it will wait for further
  // output bytes.
  strcat(message, "\n\0");
  printf("Sending: %s\n", message);

  return send_message(connfd, message);
}

// A wrapper around recv to simplify calls.
int receive_message(int connfd, char *message) {
  return recv(connfd, message, MAXLINE, 0);
}

// Helper function to establish an open listening socket on given port.
int open_listenfd(int port) {
  int listenfd;    // the listening file descriptor.
  int optval = 1;  //
  struct sockaddr_in serveraddr;

  /* Create a socket descriptor */
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;

  /* Eliminates "Address already in use" error from bind */
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval,
                 sizeof(int)) < 0)
    return -1;

  /* Listenfd will be an endpoint for all requests to port
     on any IP address for this host */
  bzero((char *)&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)port);
  if (bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0) return -1;

  /* Make it a listening socket ready to accept connection requests */
  if (listen(listenfd, LISTENQ) < 0) return -1;
  return listenfd;
}

// thread function prototype as we have a forward reference in main.
void *thread(void *vargp);

int main(int argc, char **argv) {
  // Check the program arguments and print usage if necessary.
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }

  // initialize message buffer.
  init_message_buf();

  // Initialize the message buffer lock.
  pthread_mutex_init(&lock, NULL);

  // The port number for this server.
  int port = atoi(argv[1]);
  // The listening file descriptor.
  int listenfd = open_listenfd(port);

  //create the default room
  room default_room;
  //add the room to the list of rooms
  room_list.push_back(&default_room);

  // The main server loop - runs forever...
  printf("Loop starting\n");
  while (1){
    //file descriptor for the connection
    int *connfdp = (int*)(malloc(sizeof(int)));
    //client IP info variable
    struct sockaddr_in clientaddr;
    // Wait for incoming connections.
    socklen_t clientlen = sizeof(struct sockaddr_in);
    *connfdp = accept(listenfd, (SA *)&clientaddr, &clientlen);
    // determine the domain name and IP address of the client
    struct hostent *hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
                      sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    // The server IP address information.
    char *haddrp = inet_ntoa(clientaddr.sin_addr);
    // The client's port number.
    unsigned short client_port = ntohs(clientaddr.sin_port);
    printf("server connected to %s (%s), port %u\n", hp->h_name, haddrp, client_port);
    // Create a new thread to handle the connection.
    pthread_t tid;
    pthread_create(&tid, NULL, thread, connfdp);
    }
  printf("Server ending\n");
}


//method to send messages to all users within a room
void send_message_to_room(int connfd, char* message, user* cur_user){
  printf("BEGINNING OF send_message_to_room\n");
  //get the room the user is in
  room* users_room = cur_user->cur_room;
  //get the user list within this room
  std::vector<user*> users = users_room->user_list;
  //send the message to each user in this room
  printf("users size: %d\n", users.size());
  for(std::vector<user*>::iterator it = users.begin(); it != users.end(); it++){
    //get this users connection file descriptor
    printf("SENDINGSENDINGSENDINGSENDINGSENDINGSENDINGSENDINGSENDINGSENDINGSENDINGSENDING\n");
    int cur_fd = (*it)->connection_fd;
    printf("User nickname: %s\n", (*it)->nickname);
    char temp_buf[MAXLINE];
    sprintf(temp_buf, "%s: %s", cur_user->nickname, message);
    send(cur_fd, temp_buf, strlen(temp_buf), 0);
  }
}

//method for threads to execute, loop for getting/sending messages
void handle_messages(int connfd, user* cur_user){
  printf("IN HANDLE_MESSAGES\n");
  size_t n;

  // Holds the received message.
  char message[MAXLINE];

  while ((n = receive_message(connfd, message)) > 0) {
    printf("RECEIVED MESSAGE STARTING\n");
    message[n] = '\0';  // null terminate message (for string operations)
    printf("Server received message %s (%d bytes)\n", message, (int)n);
    //add the current message to the circular buffer
    add_message(message);

    //send message to all users in the room of the user
    send_message_to_room(connfd, message, cur_user);
    //send(connfd, message, strlen(message), 0);
  }
}




/*
// Destructively modify string to be upper case
void upper_case(char *s) {
  while (*s) {
    *s = toupper(*s);
    s++;
  }
}

int send_echo_message(int connfd, char *message) {
  upper_case(message);
  add_message(message);
  return send_message(connfd, message);
}

int process_message(int connfd, char *message) {
  if (is_list_message(message)) {
    printf("Server responding with list response.\n");
    return send_list_message(connfd);
  } else {
    printf("Server responding with echo response.\n");
    return send_echo_message(connfd, message);
  }
}

// The main function that each thread will execute.
void echo(int connfd) {
  size_t n;

  // Holds the received message.
  char message[MAXLINE];

  while ((n = receive_message(connfd, message)) > 0) {
    message[n] = '\0';  // null terminate message (for string operations)
    printf("Server received message %s (%d bytes)\n", message, (int)n);
    n = process_message(connfd, message);
  }
}*/








/* thread routine */
void *thread(void *vargp) {
  printf("THREADTHREADTHREADTHREADTHREADTHREADTHREADTHREADTHREADTHREAD\n");
  // Grab the connection file descriptor.
  int connfd = *((int *)vargp);
  // Detach the thread to self reap.
  pthread_detach(pthread_self());
  // Free the incoming argument - allocated in the main thread.
  free(vargp);

  //set up user struct
  char message[16];
  int n = receive_message(connfd, message);
  message[n] = '\0';
  user cur_user;
  strcpy(cur_user.nickname, message);
  //printf("%s\n", message);
  printf("username: %s\n", cur_user.nickname);
  //printf("Server received message %s (%d bytes)\n", cur_user.nickname, (int)n);
  cur_user.connection_fd = connfd;
  //assign the user the default room and add them to the list of users within that room
  cur_user.cur_room = room_list.front();
  ((room_list.front())->user_list).push_back(&cur_user);

  handle_messages(connfd, &cur_user);
  //echo(connfd);
  //delete the user from the room it is in
  //find the position of the user's room in the room list
  room* users_cur_room = cur_user.cur_room;
  printf("users_cur_room's user list size: %d\n", ((users_cur_room)->user_list).size());
  std::vector<room*>::iterator room_list_pos = std::find(room_list.begin(), room_list.end(), users_cur_room);
  //check to see if we found the room where the user is in
  if (room_list_pos != room_list.end()){
    printf("USER'S ROOM FOUND AT POSITION:\n");
    //delete the user from the user_list in the room
    std::vector<user*>::iterator room_user_list_pos = std::find((users_cur_room->user_list).begin(), (users_cur_room->user_list).end(), &cur_user);
    //check to see if we found the user in this room's user list
    if (room_user_list_pos != (users_cur_room->user_list).end()){
      printf("USER FOUND AT POSITION:\n");
      //delete the user from this room's user list
      (users_cur_room->user_list).erase(room_user_list_pos);
    }
  }
  printf("users_cur_room's user list size: %d\n", ((users_cur_room)->user_list).size());

  printf("client disconnected.\n");
  // Don't forget to close the connection!
  close(connfd);
  return NULL;
}
