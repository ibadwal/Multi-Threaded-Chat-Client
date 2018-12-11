from socket import *
import threading
import sys
from cconsole import print_string, set_input_function, alive

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

connected = False
connection = None
		
def client():
	global connected
	global connection
	
	def console_input(input_string):
		global connected
		global connection
	
		if connected:
			send(connection, input_string)
		else:
			try:
				connection = connect()
				connected = True
			except:
				print_string("Server is not online, press Enter to reconnect.")
	
	def console_output():
		global connected
		global connection
	
		while(alive()):
			try:
				response = recv(connection)
				if response != "":
					print_string(response)
				else:
					if connected:
						connected = False
						print_string("Server died! Press Enter to reconnect.")
			except:
				pass
		print()
		sys.exit(0)
	
	set_input_function(console_input)
	
	try:
		connection = connect()
		connected = True
	except:
		print_string("Server is not online, press Enter to reconnect.")	
	
	reply_thread = threading.Thread(target=console_output)
	reply_thread.start()
	reply_thread.join()		
		
client()
