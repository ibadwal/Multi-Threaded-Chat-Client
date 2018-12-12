import sys
import time
import threading
import os

"""
Getch commands provided by ActiveState.com
"""
class _Getch:
	"""Gets a single character from standard input. Does not echo to the screen."""
	def __init__(self):
		try:
			self.impl = _GetchWindows()
		except ImportError:
			self.impl = _GetchUnix()

	def __call__(self): return self.impl()


class _GetchUnix:
	def __init__(self):
		import tty, sys

	def __call__(self):
		import sys, tty, termios
		fd = sys.stdin.fileno()
		old_settings = termios.tcgetattr(fd)
		try:
			tty.setraw(sys.stdin.fileno())
			ch = sys.stdin.read(1)
		finally:
			termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
		return ch


class _GetchWindows:
	def __init__(self):
		import msvcrt

	def __call__(self):
		import msvcrt
		ch = msvcrt.getch()
		try:
			return ch.decode()
		except:
			return


getch = _Getch()



input_string = ""
console_hint = ">>> "
_quit = False
input_function = None

def set_input_function(new_input_function):
	global input_function
	input_function = new_input_function

def print_string(string):
	global input_string	
	sys.stdout.write('\r' + string)
	
	padding = " "*(len(input_string) + len(console_hint) - len(string))
	
	sys.stdout.write(padding)
	sys.stdout.write("\n" + console_hint + input_string)

def t_input():
	global input_string
	global _quit
	pads = 0
	while not _quit:
		c = getch()
		if c == None:
			continue
		if '\r' in c:
			if input_string == "quit":
				quit()
				return
	
			input_string_copy = str(input_string)				
			sys.stdout.write('\r' + console_hint + " "*len(input_string))			
			input_string = ""
			pads = 0
			if input_function != None:
				input_function(input_string_copy)
			
		else:
			if '\x08' in c or '\x7f' in c:
				input_string = input_string[:-1]
				pads += 1
			else:
				input_string += c
				pads = max(0, pads-1)
			sys.stdout.write('\r' + console_hint + input_string + " "*pads)
	
print_string("")
	
input_thread = threading.Thread(target=t_input)
input_thread.start()

def alive():
	return input_thread.is_alive()

def quit():
	_quit = True
	sys.stdout.write("\n\n\n")
	sys.exit()
