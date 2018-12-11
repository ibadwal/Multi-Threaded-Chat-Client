from socket import *
import threading
import sys
from cconsole import print_string, set_input_function, _quit

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

def client():	
	def console_input(input_string):
		if input_string == "quit":
			pass#sys.exit(0)
		else:
			send(connection, input_string)
	
	def console_output():
		while(not _quit):
			try:
				response = recv(connection)
				print_string(response)
			except:
				pass
		sys.exit(0)
	
	set_input_function(console_input)
	connection = connect()
	
	reply_thread = threading.Thread(target=console_output)
	reply_thread.start()
	reply_thread.join()		
		
client()
