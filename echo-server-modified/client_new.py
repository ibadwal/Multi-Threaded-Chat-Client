from socket import *
import threading
import sys

SERVER_NAME = 'localhost'
SERVER_PORT = 3000


def connect():
    # Create a socket of Address family AF_INET.
    sock = socket(AF_INET, SOCK_STREAM)
    # Client socket connection to the server
    sock.connect((SERVER_NAME, SERVER_PORT))

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


def ask(prompt=':-p'):
    return input(f'({prompt}) ')


def prompt_on_last(sock):
    last = last_list(sock)
    if last == '':
        return ask()
    else:
        return ask(last)

#thread function for reading data from the server
def read_thread(connection):
  while True:
    response = recv(connection)
    #sys.stdout.write('Message:')
    #print()
    print(response)

#thread function for writing data to the server
def write_thread(connection):
  sentence = ask()
  while True:
    send(connection, sentence)
    #response = recv(connection)
    #print(response.strip())
    sentence = ask()

def client():
    connection = connect()
    
    #set up and send nickname data to the server
    nickname = ask("What is your username?")
    while len(nickname) > 16:
      nickname = ask("Your username must be less than 17 characters. Choose another:")
    print("nickname: " + nickname)
    send(connection, nickname)

    #print("HERE WE GO")
    #sentence = ask("Message:")
    #while sentence != 'quit':
    #  print("KEEP GOING")
    #  send(connection, sentence)
    #  response = recv(connection)
    #  print(response.strip())
    #  sentence = ask("Message:")

    #create the threads for reading and writing
    thread_for_read = threading.Thread(target=read_thread, args=(connection,));
    thread_for_write = threading.Thread(target=write_thread, args=(connection,));

    #start the threads for reading and writing
    thread_for_read.start();
    thread_for_write.start();

    #wait until both of the threads completely finish
    thread_for_read.join();
    thread_for_write.join();

client()
