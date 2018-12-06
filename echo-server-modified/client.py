from socket import *
import threading

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
def read_thread():
  response = recv(connection)

#thread function for writing data to the server
def write_thread():
  while sentence != 'quit':
    send(connection, sentence)
    response = recv(connection)
    print(response.strip())
    sentence = prompt_on_last(connection)

def client():
    connection = connect()
    
    #set up and send nickname data to the server
    nickname = ask("What is your username?")
    while len(nickname) > 16:
      nickname = ask("Your username must be less than 17 characters. Choose another:")
    print("nickname: " + nickname)
    send(connection, nickname)

    #set up the threads

client()
