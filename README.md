# 377-Final-Project
Final for CS377

TODO:
- Support multiple clients connecting to server [NOT STARTED]
		- Concurrency + Threads
		- Each client connecting to the server is a thread
		- Support reading/writing of socket from multiple threads (Assume socket provided by OS is thread safe)
- User (Struct with info) [NOT STARTED]
		- Associated with a nickname
		- Associated with a socket connection to server
- Chat Rooms [NOT STARTED]
		- Chat server that supports multiple chat rooms
		- Way to implement: Data structure for a chat room which holds a list of user structs
		- Users can join/leave chat rooms (allow list to change over time)
- Chat Protocol Commands [NOT STARTED]
		- \JOIN {nickname} {room}
		When the server receives this command it will add the user to the list of users associated with a particular room. If the room does not exist it will create a new room and add the user to the list of users associated with the new room. The server must respond to the client with the name of the room.
		- \ROOMS
		When the server receives this command it will respond with a list of the available rooms.
		- \LEAVE
		When the server receives this command it will remove the user from the room and, send the single message GOODBYE, and disconnect the client.
		- \WHO
		When the server receives this command it will send a list the users in the room the user is currently in.
		- \HELP
		When the server receives this command it will send a list out the available commands.
		- \{nickname} {message}
		When the server receives this command it will send the message to the user specified by nickname. Note #1: you will also need to echo the command back to the sender so it is displayed on the client. Note #2: you will need to use a mutex to synchronize access to the socket of the target user and sending client when you write to their sockets.
		If a command is sent to the server that is not recognized (e.g., \LAEVE), the server must respond with: “\LAEVE” command not recognized.
		All other messages sent to the server that do not match a command must be broadcasted to each of the connected clients including the sender. When displayed on the client, the message should be in the form:
		nickname: message
		One approach to this is to simply iterate over all users in a room sending the message to each user (over their associated socket). Note: you will need to use a mutex to synchronize access to each socket before writing to it.
-	Testing [NOT STARTED]
		- Scripting (files with commands etc.)
-	Added Functionality [NOT STARTED]
		- Add a new feature not listed already for basic features
- Video [NOT STARTED]