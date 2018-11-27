import sys
from socket import *

# Helper function to check if input is an integer
def is_int(string):
    try: 
        int(string)
        return True
    except:
        return False

try:

	# Allowed input operations
	operations = ["+", "-", "*", "/"]

	# Port that we will communicate on
	server_port = 50105

	# Create socket for TCP communication on our port
	socket = socket(AF_INET, SOCK_STREAM)
	socket.bind(("", server_port))
	
	# Start listening for client input
	socket.listen(1)
	
	print("Server Running, ready to receive data.")

	# Loop forever
	while True:
	
		# Accept connection with client, create new socket
		client_socket, client_address = socket.accept()
		
		# Receive client input message
		message = client_socket.recv(128)
		
		print("poop")
		
		# Decode input, split by spaces
		input = message.decode().split(" ")
		
		# If input is valid
		if len(input) == 3 and input[0] in operations and is_int(input[1]) and is_int(input[2]):
			
			# Perform operations
			if input[0] == "+":
				output = str(int(input[1]) + int(input[2]))
			elif input[0] == "-":
				output = str(int(input[1]) - int(input[2]))
			elif input[0] == "*":
				output = str(int(input[1]) * int(input[2]))			
			elif input[0] == "/":
				output = str(int(input[1]) / int(input[2]))
			output = "200 " + output
		
		# If input is not valid
		else:
			output = "300 -1"	
		
		# Print information to console
		print("Received input: " + str(input))
		print("Output is: " + output)
		
		# Send output to client
		client_socket.send(output.encode())
		
		# Close socket
		client_socket.close()
	
# Handle exception if program fails
except:

	# Close the welcoming socket
	socket.close()
	
	print("Exception Occurred, restart program.")