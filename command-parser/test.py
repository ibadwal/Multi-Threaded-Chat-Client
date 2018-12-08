import sys
import time
import threading
import os

"""
Getch commands provided by ActiveState.com
"""
class _Getch:
	"""Gets a single character from standard input.  Does not echo to the
screen."""
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
				_quit = True
				return
	
			input_string_copy = str(input_string)
			input_string = ""		
			sys.stdout.write('\r' + input_string_copy)
			padding = " "*len(console_hint)
			sys.stdout.write(padding)
			sys.stdout.write("\n" + console_hint + input_string)
			pads = 0
		else:
			if '\x08' in c or '\x7f' in c:
				input_string = input_string[:-1]
				pads += 1
			else:
				input_string += c
				pads = max(0, pads-1)
			sys.stdout.write('\r' + console_hint + input_string + " "*pads)
	
def t_output():
	while not _quit:
		time.sleep(5)
		print_string("INTERUPTING COW MOO")

print_string("")
	
input_thread = threading.Thread(target=t_input)
input_thread.start()
output_thread = threading.Thread(target=t_output)
output_thread.start()
input_thread.join()
output_thread.join()