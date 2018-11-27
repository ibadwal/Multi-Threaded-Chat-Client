import sys
from socket import *

#try:

# Server address and port
server_name = "127.0.0.1"
server_port = 50105

# Loop forever
while True:

	# Get input from console
	inpt = input("Input: ")
	while inpt == '':
		inpt = input("Input: ")
	
	# Check if user wants to quit
	if inpt == "quit":
		sys.exit()
	
	# Create socket for TCP communication with server
	server_socket = socket(AF_INET, SOCK_STREAM)
	server_socket.connect((server_name, server_port))
		
	# Send input to server
	server_socket.send(inpt.encode())
	
	# Receive response from server
	message = server_socket.recv(128)
	
	# Decode response, split into status code and output
	output = message.decode().split(" ")
	
	# If output is good
	if int(output[0]) == 200:
		print("Output: " + output[1])
	
	# If output is bad
	else:
		print("Invalid Syntax, use [operation] [integer 1] [integer 2].")
	
	# Close the socket
	server_socket.close()

# Handle exceptions		
#except:
#	print("Program ended.")