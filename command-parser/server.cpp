#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

// compile and run with
// g++ server.cpp -o server; ./server client_commands.txt

using namespace std;

void parse_input(string);
string rainbow(string);

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

int main(int argc, char *argv[]){
	if(argc == 1){
		printf("Please specify in input file.\n");
		exit(0);
	}
	
	ifstream input;
	input.open(argv[1]);
		
	while(input.good()){
		char buffer[64];
		input.getline(buffer, 64);
		string input(buffer);
		cout << CYAN << "Parsing Input: " << RESET << input << "\n"; //TEMPORARY
		parse_input(input);
	}
		
	input.close();
}

void help(){
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
	output += command_color + "\\LEAVE" + info_color + "\n\tLeaves the current room.\n";
	
	cout << output; //TEMPORARY	
	
	//TODO: send "output" to current user
}

void who(){
	cout << MAGENTA << "[WHO] Displaying all users in the room" << RESET << "\n"; //TEMPORARY
	
	//TODO: get list of users and send it to current use	
	
}

void rooms(){
	cout << MAGENTA << "[ROOMS] Listing rooms" << RESET << "\n"; //TEMPORARY
	
	//TODO: get list of rooms and send it to current user
	
}

void leave(){
	cout << MAGENTA << "[LEAVE] Leaving room" << RESET << "\n"; //TEMPORARY
	
	//TODO: remove the current user from current room and send them "GOODBYE"
	
}

void join(string nickname, string room_name){
	cout << MAGENTA << "[JOIN] Joining room " << RESET << room_name << MAGENTA 
		<< " as " << RESET << nickname << "\n"; //TEMPORARY	

	//TODO: add the current user to the specified room
	
}

void say(string message){
	cout << YELLOW << "[PUBLIC] User said: " << RESET << message << "\n"; //TEMPORARY
	
	//TODO: send the message to the current room witht the current user's name attatched
	
}

void direct_message(string recipient, string message){
	cout << YELLOW << "[DM] User messaged " << RESET << message
		<< YELLOW << ": " << RESET << recipient << "\n"; //TEMPORARY
	
	//TODO: send the recipient and the current user the message
	
}

void invalid_command(){
	cout << RED << "ERROR: Invalid command." << RESET << "\n"; //TEMPORARY
	
	//TODO: send the current user a message alerting them of the invalid command
	
}

/*
	parse_input
	
	Arguments: 
		- (string) input : The input string (C++ string)

	TODO:
		- Adding extra arguments does not always cause an error
*/
void parse_input(string input){
	
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
			help();
		} else if(input.substr(0, 6).compare("\\ROOMS") == 0){ //HELP command
			rooms();
		} else if(input.substr(0, 4).compare("\\WHO") == 0){ //WHO command
			who();
		} else if(input.substr(0, 6).compare("\\LEAVE") == 0){ //LEAVE command
			leave();
		} else if(input.substr(0, 5).compare("\\JOIN") == 0){ //JOIN Command
			
			// Find index of the space before second argument (index of first argument is 6)
			int index_arg2 = input.find(' ', 6);
			
			//Make sure arguments are good
			if(index_arg2 == -1 or index_arg2 == 6 or input.find(' ', index_arg2 + 1) != -1){
				invalid_command();
				return;
			}
			
			//Get arguments as substrings
			string nickname = input.substr(6, index_arg2 - 6);
			string room_name = input.substr(index_arg2 + 1, input.length() - index_arg2);			
	
			join(nickname, room_name);
		} else { //DIRECT MESSAGE command
			int index_message_start = input.find(' ', 1);
			if(index_message_start > 1){
				string recipient = input.substr(1, index_message_start - 1);
				string message = input.substr(index_message_start + 1,
								input.length() - index_message_start);
				direct_message(message, recipient);
			} else {
				invalid_command();
			}
		}
		
	//If input is not a command
	} else {
		say(input);
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