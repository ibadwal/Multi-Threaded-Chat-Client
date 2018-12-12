from socket import *
import sys
import time

SERVER_NAME = 'localhost'
SERVER_PORT = 8000

def connect():
	# Create a socket of Address family AF_INET.
	sock = socket(AF_INET, SOCK_STREAM)
	# Client socket connection to the server
	sock.connect((SERVER_NAME, SERVER_PORT))
	
	#sock.setblocking(0)
	sock.settimeout(0.5)
	
	return sock


def send(sock, message):
	sock.send(bytearray(message, 'utf-8'))


def recv(sock):
	return sock.recv(1024).decode('utf-8')


def list(sock):
	send(sock, '-')
	result = recv(sock).rstrip()
	if result == '':
		return []
	else:
		return result.split(',')[0:-1]


def last_list(sock):
	xs = list(sock)
	if len(xs) == 0:
		return ''
	else:
		return xs[-1]


def client(file_path):
	connection = connect()
	
	with open(file_path) as input_file:
		line = input_file.readline().rstrip()
		while line:
			#print("Input: " + line)
			if line[0] == "~":
				time.sleep(int(line[1:-1]))
				#print("Sleeping...")
			else:
				send(connection, line)
				response = recv(connection)
				print(response)
			line = input_file.readline()
				
	sys.exit(0)	
	

if len(sys.argv) > 1:
	client(sys.argv[1])
else:
	print("Please specify a command file.")
