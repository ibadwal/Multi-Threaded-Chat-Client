#include <vector>
#include <user.h>

//create a struct to represent a chat room
struct room{
  std::vector<user> user_list;  //list of all users within this room
  char room_pass[16];           //add a room_pass
};