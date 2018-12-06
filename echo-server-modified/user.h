#include <room.h>

//create a struct to represent a user
struct user{
  char nickname[16];          //nickname of a user
  int* connection_fd;         //socket connection between server and user
  room* cur_room;			        //the current room the user is in
};