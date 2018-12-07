import msvcrt
import sys
import time
import threading

input_string = ""
console_hint = ">>> "

def print_string(string):
	global input_string	
	sys.stdout.write('\r' + string)
	#if len(string) < len(input_string) + len(console_hint):
	padding = " "*(len(input_string) + len(console_hint) - len(string))
	
	#print()
	#print(len(input_string))# + len(console_hint) - len(string))
	#print()
	sys.stdout.write(padding)
	sys.stdout.write("\n" + console_hint + input_string)

def t_input():
	global input_string
	pads = 0
	while True:
		c = msvcrt.getch()
		if '\\r' in str(c):
			input_string_copy = str(input_string)
			input_string = ""		
			sys.stdout.write('\r' + input_string_copy)
			padding = " "*len(console_hint)
			sys.stdout.write(padding)
			sys.stdout.write("\n" + console_hint + input_string)
			pads = 0
		else:
			if '\\x08' in str(c):
				input_string = input_string[:-1]
				pads += 1
			else:
				input_string += str(c)[2]
				pads = max(0, pads-1)
			sys.stdout.write('\r' + console_hint + input_string + " "*pads)
	
def t_output():
	while True:
		time.sleep(5)
		print_string("INTERUPTING COW MOO")

print_string("")
		
input_thread = threading.Thread(target=t_input)
input_thread.start()
output_thread = threading.Thread(target=t_output)
output_thread.start()
