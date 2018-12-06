#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

void parse_input(string);

const string YELLOW = "\033[1;33m";
const string MAGENTA = "\033[1;35m";
const string CYAN = "\033[1;36m";
const string RED = "\033[1;31m";
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
	//TODO
	
}

void who(){
	cout << MAGENTA << "[WHO] Displaying all users in the room" << RESET << "\n"; //TEMPORARY
	//TODO
	
}

void rooms(){
	cout << MAGENTA << "[ROOMS] Listing rooms" << RESET << "\n"; //TEMPORARY
	//TODO
	
}

void leave(){
	cout << MAGENTA << "[LEAVE] Leaving room" << RESET << "\n"; //TEMPORARY
	//TODO
	
}

void join(string nickname, string room_name){
	cout << MAGENTA << "[JOIN] Joining as " << RESET << nickname << MAGENTA 
		<< " in room " << RESET << room_name << "\n"; //TEMPORARY	
	//TODO
	
}

void say(string message){
	cout << YELLOW << "User said: " << RESET << message << "\n"; //TEMPORARY
	//TODO
	
}

void invalid_command(){
	cout << RED << "ERROR: Invalid command." << RESET << "\n"; //TEMPORARY
	//TODO
	
}

/*
	parse_input
	
	Arguments: 
		- (string) input : The input string (C++ string)

	TODO:
		- Adding extra arguments does not always cause an error

*/
void parse_input(string input){
	if(input[0] == '\\'){
		if(input.substr(0, 5).compare("\\HELP") == 0){
			help();
		} else if(input.substr(0, 6).compare("\\ROOMS") == 0){
			rooms();
		} else if(input.substr(0, 4).compare("\\WHO") == 0){
			who();
		} else if(input.substr(0, 6).compare("\\LEAVE") == 0){
			leave();
		} else if(input.substr(0, 5).compare("\\JOIN") == 0){
			
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
		} else {
			invalid_command();
		}
	} else {
		say(input);
	}
}