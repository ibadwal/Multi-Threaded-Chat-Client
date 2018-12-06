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
  std::vector<user> user_list;  //list of all users within this room
  char room_pass[16];           //add a room_pass
};


/* Simplifies calls to bind(), connect(), and accept() */
typedef struct sockaddr SA;

/* Max text line length */
#define MAXLINE 8192

/* Second argument to listen() */
#define LISTENQ 1024

//initialize a vector of rooms that will be used as chat rooms
std::vector<room> room_list;

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
  printf("Sending: %s", message);

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
  //room new_room;
  //add the room to the list of rooms
  //room_list.push_back(&new_room);

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

/* thread routine */
void *thread(void *vargp) {
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



  printf("client disconnected.\n");
  // Don't forget to close the connection!
  close(connfd);
  return NULL;
}
