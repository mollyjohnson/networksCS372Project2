#!/usr/bin/env python3

#although this file doesn't have the .py extension, it is written in python and the above shebang was included
#to make sure it will be run using python 3 on the flip server. Adapted from my own work on CS 372 intro to 
#networks Project 1.
#Programmer Name: Molly Johnson (johnsmol)
#Program name: ftclient
#Program description:
#Course name: CS 372 Intro to Networks
#Last modified:

#imported libraries for socket (to create socket and use it) and sys (system, used to get input from the user)
from socket import *
import sys

NUM_ARGS = 6
MAX_PORT = 65535
MIN_PORT = 1025
LIST_COMMAND = "-l"
GET_COMMAND = "-g"
SERVER_HOST_ADDRESS = "flip1.engr.oregonstate.edu"
CLIENT_HOST_ADDRESS = "flip2.engr.oregonstate.edu"

#pre-conditions:
#post-conditions:
#description:
def ArgCheck():
	if (len(sys.argv) != NUM_ARGS):
		print("Wrong number of arguments! Must enter server host, server port num, commmand, filename, and data port num. Start the program again.")
		exit()
	#else if there's the right nuber of arguments, make sure the argument is a valid port number.
	#using isdigit() to check if a string represents a non-negative integer adapted from:
	#https://stackoverflow.com/questions/1265665/how-can-i-check-if-a-string-represents-an-int-without-using-try-except
	if ((sys.argv[2].isdigit() == False) or (sys.argv[5].isdigit() == False)):
		print("You entered a negative integer or string for a port number. Must enter a valid port number (1025-65535). Start the program again.")
		exit()
	#if a non-neg integer was entered, check it's in valid port num range. valid port number range: 1025-65535
	#valid port values excerpted from:
	#https://www.webopedia.com/quick_ref/portnumbers.asp
	if ((int(sys.argv[2]) > MAX_PORT) or (int(sys.argv[5]) > MAX_PORT) or (int(sys.argv[2]) < MIN_PORT) or (int(sys.argv[5]) < MIN_PORT)):
		print("You entered a port number outside the valid port number range (1025-65535). Must enter a valid port number. Start the program again.")
		exit()
	if ((int(sys.argv[2])) == (int(sys.argv[5]))):
		print("You entered the same port number for the control port and data port. This is not allowed. Start the program again.")
		exit()
	if(sys.argv[1] != SERVER_HOST_ADDRESS):
		print("You didn't enter the correct server host address. Make sure to use " + SERVER_HOST_ADDRESS + " for the server host and start the program again.")
		exit()

#pre-conditions:
#post-conditions:
#description:
def InitiateContact(portNum, hostName):
	sockFD = socket(AF_INET, SOCK_STREAM)
	sockFD.connect((hostName, portNum))

	return sockFD

#pre-conditions:
#post-conditions:
#description:
def main():
	#check num and validity of the command line args
	ArgCheck()
	controlPort = int(sys.argv[2])
	serverHost = sys.argv[1]
	command = sys.argv[3]
	filename = sys.argv[4]
	dataPort = int(sys.argv[5])
	#delimiter = chr(216) + chr(216) + chr(216)
	delimiter = chr(3) + chr(3) + chr(3)
	controlMessage = (command + delimiter + filename)

	socketFDControl = InitiateContact(controlPort, serverHost)

	socketFDControl.send(controlMessage.encode())

	socketFDControl.close()

#used to call the main function
#using a main() function in python excerpted from:
#https://www.journaldev.com/17752/python-main-function
if __name__ == '__main__':
	main()
