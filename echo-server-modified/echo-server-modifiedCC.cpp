/*
 * echoserverts.c - A concurrent echo server using threads
 * and a message buffer.
 
 compile with:
 g++ echo-server-modifiedCC.cpp -o server -lpthread
 
 */
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

const string BLACK = "\033[1;30m";
const string RED = "\033[1;31m";
const string GREEN = "\033[1;32m";
const string YELLOW = "\033[1;33m";
const string BLUE = "\033[1;34m";
const string MAGENTA = "\033[1;35m";
const string CYAN = "\033[1;36m";
const string WHITE = "\033[1;37m";
const string GRAY = "\033[0;37m";
const string GOLD = "\033[0;33m";
const string DARK_RED = "\033[0;31m";
const string PURPLE = "\033[0;35m";
const string DARK_GREEN = "\033[0;32m";
const string RESET = "\033[0m";

struct room;

//create a struct to represent a user
struct user {
	string nickname;					//nickname of a user
	int connection_fd;					//socket connection between server and user
	room* curr_room;						 //the current room the user is in

	bool operator==(const user& incoming_user) const {
		return (nickname.compare(incoming_user.nickname) == 0);
	}
};

//create a struct to represent a chat room
struct room {
	string id; 						//ID of the room
	std::vector<user> user_list;	//list of all users within this room
	char room_pass[16];				//add a room_pass

	bool operator==(const room& incoming_room) const {
		return (id.compare(incoming_room.id) == 0);
	}
};

void parse_input(user*, string);
string rainbow(string);

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

// A wrapper for sending c++ strings
int send_string(user* recipient, string message) {
	return send_message(recipient->connection_fd, (char*)message.c_str());
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
	int listenfd;		// the listening file descriptor.
	int optval = 1;	//
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

	//TEMPORARY
	room sonic_fan_club;
	sonic_fan_club.id = "sonic";
	room_list.push_back(sonic_fan_club);

	
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

void broadcast(room*, string);

/* thread routine */
void *thread(void *vargp) {
	// Grab the connection file descriptor.
	int connfd = *((int *)vargp);
	// Detach the thread to self reap.
	pthread_detach(pthread_self());
	// Free the incoming argument - allocated in the main thread.
	free(vargp);
	
	user u;		
	user* curr_user = &u;
	curr_user->connection_fd = connfd;
	curr_user->curr_room = NULL;
	
	send_string(curr_user, "Connected to the Chat Server. Type \\HELP for commands.");
	
	while(true){
		char message[128];
		int n = receive_message(connfd, message);
		message[n] = '\0';			
		if((int)n > 0){
			printf("Server received message \"%s\" (%d bytes)\n", message, (int)n);
			string message_string(message);
			parse_input(curr_user, message_string);
		} else {			
			break;
		}
	}
	
	printf("client disconnected.\n");
	// Don't forget to close the connection!
	close(connfd);
	return NULL;
}

void help(user* sender){
	cout << MAGENTA << "[HELP] Displaying help information" << RESET << "\n"; //TEMPORARY
	
	string command_color = GREEN;
	string argument_color = YELLOW;
	string info_color = WHITE;
	
	string output = "";
	output += command_color + "\\JOIN " + argument_color + "{your nickname} {room name}"
			+ info_color + "\n\tJoins a room as the specified nickname.\n";
	output += command_color + "\\ROOMS" + info_color + "\n\tDisplays a list of all open rooms.\n";
	output += command_color + "\\WHO" + info_color + "\n\tDisplays a list of all users in the room.\n";
	output += command_color + "\\HELP" + info_color + "\n\tDisplays a list of all commands.\n";	
	output += command_color + "\\" + argument_color + "{recipient's nickname} {private message}"
			+ info_color + "\n\tSends the recipient a private message.\n";
	output += command_color + "\\LEAVE" + info_color + "\n\tLeaves the current room." + RESET;
	
	//cout << output; //TEMPORARY	
	
	send_string(sender, output);
}

void who(user* sender){
	cout << MAGENTA << "[WHO] Displaying all users in the room" << RESET << "\n"; //TEMPORARY
		
		//TODO: get list of users and send it to current user
		string output;
		if(sender->curr_room == NULL){
			output = RED;
			output += "ERROR: you have not joined a room!";
			output += RESET;
			send_string(sender, output);
			return;
		}
		std::vector<user> curusers = (sender->curr_room)->user_list;
		output = GREEN;
		output += "Users in " + (sender->curr_room)->id + ": ";
		output += YELLOW;
		for(vector<user>::iterator i = curusers.begin(); i!= curusers.end(); i++){
			output += i->nickname + ", ";
		}
		output.erase(output.size()-2, output.size());
		output += RESET;
		send_string(sender, output);
}

void rooms(user* sender){
	cout << MAGENTA << "[ROOMS] Listing rooms" << RESET << "\n"; //TEMPORARY
		string output = GREEN;
		if(room_list.size() == 0){
			output += "There are no active rooms. Use \\JOIN to create one!";
		} else {
			output += "Active Rooms: " + YELLOW;
			for(vector<room>::iterator i = room_list.begin(); i != room_list.end(); i++){
					output += i->id + ", ";
			}
			output.erase(output.size()-2, output.size());		
		}
		output += RESET;
		send_string(sender, output);
}

void leave(user* sender){
	cout << MAGENTA << "[LEAVE] Leaving room" << RESET << "\n"; //TEMPORARY
	//cout << (*sender).nickname << endl;
	//TODO: remove the current user from current room and send them "GOODBYE"
	room* users_cur_room = sender->curr_room;
	//cout << "room size: " << ((users_cur_room)->user_list).size() << endl;
	printf("users_cur_room's user list size: %d\n", (int)((users_cur_room)->user_list).size());
	std::vector<room>::iterator room_list_pos = std::find(room_list.begin(), room_list.end(), *users_cur_room);
	//check to see if we found the room where the user is in
	if (room_list_pos != room_list.end()){
		//delete the user from the user_list in the room
		std::vector<user>::iterator room_user_list_pos = std::find((users_cur_room->user_list).begin(), (users_cur_room->user_list).end(), *sender);
		//check to see if we found the user in this room's user list
		if (room_user_list_pos != (users_cur_room->user_list).end()){
			//delete the user from this room's user list
			(users_cur_room->user_list).erase(room_user_list_pos);
		}
	}
	(*sender).curr_room = NULL;
	printf("users_cur_room's user list size: %d\n", (int)((users_cur_room)->user_list).size());
	send_string(sender, "Goodbye! Leaving current room.");
	//cout << "room size: " << ((users_cur_room)->user_list).size() << endl;
}

void join(user* sender, string nickname, string room_name){
	cout << MAGENTA << "[JOIN] Joining room " << RESET << room_name << MAGENTA 
		<< " as " << RESET << nickname << "\n"; //TEMPORARY	
	
	//check if user is already in a room and remove them if they are
	//first check if the user's room exists already (already in a room)
	if (sender->curr_room != NULL){
		//if they are in a room, we want them to leave that room
		//reuse the leave(user* sender) function here:
		//leave gets rid of user in the user's room and clears the user's room field
		leave(sender);
	}
	
	if(nickname.length() > 0 and nickname.length() <= 16){
		bool found_room = false;
		for(std::vector<room>::iterator i = room_list.begin(); i != room_list.end(); i++){
			if(i->id.compare(room_name) == 0){				
				found_room = true;
				//check if nickname exists in room already
				std::vector<user> temp_user_list = (&(*i))->user_list;
				//std::vector<user>::iterator nickname_pos_in_room_list = std::find((i->user_list).begin(), (i->user_list).end(), *sender);
				bool duplicate_name = false;
				for (std::vector<user>::iterator j = temp_user_list.begin(); j != temp_user_list.end(); j++){
					if (nickname.compare((&(*j))->nickname) == 0){
						//cout << "DUPE" << endl;
						duplicate_name = true;
					}
				}
				//check to see if we found the user in this room's user list
				if (duplicate_name == true){
					cout << "User's nickname is in use already." << endl;
					//tell the user that their nickname is already taken in this room - can't join this room
					string output = "";
					output += RED + "ERROR: Nickname is already in use within this room." + RESET;
					send_string(sender, output);
				}
				//end of duplicate name check
				else{
					sender->nickname = nickname;
					i->user_list.push_back(*sender);
					sender->curr_room = &(*i);
					//printf("%i\n", sender.curr_room);
					string output = YELLOW + sender->nickname + GRAY + " joined the room." + RESET;
					broadcast(&(*i), output);
				}
			}
		}
		//if we didn't find this room, create it
		if (found_room == false){
			//TODO create another room if the user's inputted room_name didn't match any existing rooms
		}
	} else {
		string output = "";
		output += RED + "ERROR: Nickname must be at most 16 characters." + RESET;
		send_string(sender, output);
	}
}

void say(user* sender, string message){
	cout << YELLOW << "[PUBLIC] User said: " << RESET << message << "\n"; //TEMPORARY
	
	if(sender->curr_room == NULL){
		send_string(sender, RED + "ERROR: You have not joined a room!" + RESET);
	} else {
		//TODO send the message to everyone in the room
		string output = YELLOW + sender->nickname + "> " + WHITE + message + RESET;
		broadcast(sender->curr_room, output);
	}	
}

void broadcast(room* rm, string message){
	for(vector<user>::iterator u = rm->user_list.begin(); u != rm->user_list.end(); u++){
		send_string(&(*u), message);
	}
}

void direct_message(user* sender, string message, string recipient){
	cout << YELLOW << "[DM] User messaged " << RESET << message
		<< YELLOW << ": " << RESET << recipient << "\n"; //TEMPORARY
	
	if(sender->nickname.compare(recipient) == 0){
		send_string(sender, RED + "ERROR: You cannot whisper to yourself!" + RESET);
		return;
	}
	
	user* u_recipient = NULL;
	for(vector<user>::iterator u = (sender->curr_room)->user_list.begin();
	u != (sender->curr_room)->user_list.end(); u++){
		if(u->nickname.compare(recipient) == 0){			
			u_recipient = &(*u);
		}
	}
	
	if(u_recipient != NULL){
		send_string(u_recipient, YELLOW + sender->nickname + GRAY + " whispered to you: " + WHITE + message + RESET);
		send_string(sender, GRAY + "You whispered to " + YELLOW + u_recipient->nickname + GRAY + ": " + WHITE + message + RESET);
	} else {
		send_string(sender, RED + "ERROR: That user is not in this room!" + RESET);
	}
}

void invalid_command(user* sender){
	string output = "";
	output += RED + "ERROR: Invalid command." + RESET;
	send_string(sender, output);
}

/*
	parse_input
	
	Arguments: 
		- (string) input : The input string (C++ string)

	TODO:
		- Adding extra arguments does not always cause an error
*/
void parse_input(user* sender, string input){
	
	//Removes newline if it is present
	if(input[input.length()-1] == '\n'){
		input.erase(input.length() - 1);
	}
	
	//Remove carriage return if it is present
	if(input[input.length()-1] == '\r'){
		input.erase(input.length() - 1);
	}
	
	//If input is command
	if(input[0] == '\\'){
		
		if(input.substr(0, 5).compare("\\HELP") == 0){
			help(sender);
		} else if(input.substr(0, 6).compare("\\ROOMS") == 0){ //HELP command
			rooms(sender);
		} else if(input.substr(0, 4).compare("\\WHO") == 0){ //WHO command
			who(sender);
		} else if(input.substr(0, 6).compare("\\LEAVE") == 0){ //LEAVE command
			leave(sender);
		} else if(input.substr(0, 5).compare("\\JOIN") == 0){ //JOIN Command
			
			// Find index of the space before second argument (index of first argument is 6)
			int index_arg2 = input.find(' ', 6);
			
			//Make sure arguments are good
			if(index_arg2 == -1 or index_arg2 == 6 or input.find(' ', index_arg2 + 1) != -1){
				invalid_command(sender);
				return;
			}
			
			//Get arguments as substrings
			string nickname = input.substr(6, index_arg2 - 6);
			string room_name = input.substr(index_arg2 + 1, input.length() - index_arg2);			
			join(sender, nickname, room_name);
		} else { //DIRECT MESSAGE command
			int index_message_start = input.find(' ', 1);
			if(index_message_start > 1){
				string recipient = input.substr(1, index_message_start - 1);
				string message = input.substr(index_message_start + 1,
								input.length() - index_message_start);
				direct_message(sender, message, recipient);
			} else {
				invalid_command(sender);
			}
		}
		
	//If input is not a command
	} else {
		say(sender, input);
	}
}

string rainbow(string input){
	string product = "";
	int c = 0;
	for(int i = 0; i < input.length(); i++){
		switch(i % 5){
			case 0:
				product += RED; break;
			case 1:
				product += YELLOW; break;
			case 2:
				product += GREEN; break;
			case 3:
				product += BLUE; break;
			case 4:
				product += MAGENTA; break;
		}
		c++;
		product += input[i];
	}
	product += RESET;
	return product;
}
