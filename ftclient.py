#!/usr/bin/env python3

#shebang included to make sure this file will be interpreted using python 3 on the flip server.
#Adapted from my own work on CS 372 intro to networks Project 1.

#Programmer Name: Molly Johnson (johnsmol)
#Program name: ftclient
#Program description: ftclient acts as the client for the control connection portion of
#this program, and as the server for the data connection portion of this program. Will
#set up a socket and listen for connections, send control info to ftserver and see
#if the message received back indicates the control info was acceptable or not and if
#it was acceptable, this program will create a server socket and wait for a data
#connection from ftserver. It will then receive either the current directory's contents
#(in the directory ftserver is in) and printed or the contents of the file previously
#specified written to a new file or overwriting a pre-existing file. Duplicate file
#names will be handled by asking the user whether they want to re-enter a filename or
#overwrite the file. Will then close the control connection and terminate.
#Course name: CS 372 Intro to Networks
#Last modified: 12-1-19

#imported libraries for socket (to create socket and use it), system (used to get input from the user),
#and os (to get list of directory contents)
from socket import *
import sys
import os

#since python doesn't really allow constant variables, listed any variables that
#shouldn't be changing and accessible by all functions similar to macro definitions
#in c or c++ as all-caps variables so I would know not to change them
MAX_NUM_ARGS = 6
MIN_NUM_ARGS = 5
#listed as valid port num range by TA Danielle on slack OSU 372 Fall 2019
MAX_PORT = 65535
MIN_PORT = 1025
LIST_COMMAND = "-l"
GET_COMMAND = "-g"
#per the assignment instructions, need to make sure ftclient and ftserver are run
#on different flip servers
SERVER_HOST_ADDRESS = "flip1.engr.oregonstate.edu"
CLIENT_HOST_ADDRESS = "flip2.engr.oregonstate.edu"
#size of 1028 listed on piazza as a good size to use that's big enough to account for
#a line of text but not too big to send with one send() call
MAX_MESSAGE_SIZE = 1028

#pre-conditions: delimiter has been initialized, number of args has been checked
#post-conditions: will set variables to their corresponding system argument, control
#connection combined message will be created w these vars plus the delimiter
#description: receives special delimiter char. assigns vars based on the number
#of args. combines the vars needed to sent to ftserver over the control connection
#into one message string, separated by the special delimiter. will return the
#variables.
def FiveArgAssignVars(delimiter):
	#set variables to their corresponding args
	controlPort = int(sys.argv[2])
	serverHost = sys.argv[1]
	command = sys.argv[3]
	dataPort = int(sys.argv[4])

	#set control message with delimiter char in between variables
	controlMessage = (command + delimiter + str(dataPort) + delimiter)

	#return all variables
	return controlPort, serverHost, command, dataPort, controlMessage

#pre-conditions: delimiter has been initialized, number of args has been checked 
#post-conditions: will set variables to their corresponding system argument, control
#connection combined message will be created w these vars plus the delimiter
#description: receives special delimiter char. assigns vars based on the number
#of args. combines the vars needed to sent to ftserver over the control connection
#into one message string, separated by the special delimiter. will return the
#variables.
def SixArgAssignVars(delimiter):
	#set variables to their corresponding args
	controlPort = int(sys.argv[2])
	serverHost = sys.argv[1]
	command = sys.argv[3]
	filename = sys.argv[4]
	dataPort = int(sys.argv[5])

	#set control message with delimiter char in between variables
	controlMessage = (command + delimiter + filename + delimiter + str(dataPort) + delimiter)

	#return all variables
	return controlPort, serverHost, command, filename, dataPort, controlMessage

#pre-conditions: num command line arguments have been checked and are 5
#post-conditions: port nums (both data and control connections) will be validated
#description: checks that the port nums (positions 2 and 4) are non-neg integers
#within the valid port num range. if they're not, will print error msg and exit. 
def FiveArgCheck():
	#if there's the right nuber of arguments, make sure the argument is a valid port number.
	#using isdigit() to check if a string represents a non-negative integer adapted from:
	#https://stackoverflow.com/questions/1265665/how-can-i-check-if-a-string-represents-an-int-without-using-try-except
	if ((sys.argv[2].isdigit() == False) or (sys.argv[4].isdigit() == False)):
		print("You entered a negative integer or string for a port number. Must enter a valid port number (1025-65535). Start the program again.")
		exit()

	#if a non-neg integer was entered, check it's in valid port num range. valid port number range: 1025-65535
	#valid port values excerpted from:
	#https://www.webopedia.com/quick_ref/portnumbers.asp
	if ((int(sys.argv[2]) > MAX_PORT) or (int(sys.argv[4]) > MAX_PORT) or (int(sys.argv[2]) < MIN_PORT) or (int(sys.argv[4]) < MIN_PORT)):
		print("You entered a port number outside the valid port number range (1025-65535). Must enter a valid port number. Start the program again.")
		exit()


#pre-conditions: num command line arguments have been checked and are 6
#post-conditions: port nums (both data and control connections) will be validated
#description: checks that the port nums (positions 2 and 5) are non-neg integers
#within the valid port num range. if they're not, will print error msg and exit.
def SixArgCheck():
	#if there's the right nuber of arguments, make sure the argument is a valid port number.
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

#pre-conditions: user has entered some command line args
#post-conditions: number of args will have been validated
#description: will check that the num of args is either 5 or 6 (the only
#two valid options). If num args is any other number, will print error msg and exit.
#if is either 5 or 6 args, will call the appropriate argCheck function to validate
#the contents of the args. will then return the num of args
def ArgNumCheck():
	#check that the number of args is either 5 (no filename) or 6 (filename)
	if ((len(sys.argv) != MAX_NUM_ARGS) and (len(sys.argv) != MIN_NUM_ARGS)):
		print("Wrong number of arguments! Must enter server host, server port num, commmand, filename (optional), and data port num. Start the program again.")
		exit()
	
	#check that the server hostname entered matches the server host address specified to be used for the server host address in the README.txt
	if(sys.argv[1] != SERVER_HOST_ADDRESS):
		print("You didn't enter the correct server host address. Make sure to use " + SERVER_HOST_ADDRESS + " for the server host and start the program again.")
		exit()
	
	#if there's 6 args, call the 6 arg function
	if (len(sys.argv) == MAX_NUM_ARGS):
		SixArgCheck()
	#if there's 5 args, call the 5 arg function
	else:
		FiveArgCheck()
	
	#return the number of args
	return len(sys.argv)

#pre-conditions: valid port num, valid host name
#post-conditions: will return a socket descriptor
#description: takes port num and hostname and creates
#tcp socket and connects. then returns the socket descriptor
def InitiateContact(portNum, hostName):
	#adapted from OSU CS 372 lecture 15 slides (specifically, slide 8)
	#create TCP socket
	sockFD = socket(AF_INET, SOCK_STREAM)

	#connect to the specified hostname and portnum
	#adapted from OSU CS 372 lecture 15 slides (specifically, slide 8)
	sockFD.connect((hostName, portNum))

	#return socket file descriptor
	return sockFD

#pre-conditions: valid socket descriptor, valid message
#post-conditions: will have sent the message
#description: accepts socket descriptor and a message, uses send()
#to send the message to the socket via socket descriptor
def MakeRequest(sockFD, message):
	#adapted from OSU CS 372 lecture 15 slides (specifically, slide 9)
	#and p.205 from Computer Networking-A Top-Down Approach by Kurose and Ross, 7th ed
	#send the specified message using send()
	sockFD.send((message).encode())

#pre-conditions: valid socket descriptor
#post-conditions: will receive a message and return it
#description: accepts socket descriptor and uses recv() to receive
#the message using socket. returns message
def ReceiveDataControl(sockFD):
	#adapted from OSU CS 372 lecture 15 slides (specifically, slide 9)
	#and p.205 from Computer Networking-A Top-Down Approach by Kurose and Ross, 7th ed
	#receive the message from the socket using recv()
	message = sockFD.recv(MAX_MESSAGE_SIZE).decode()

	#return the message received
	return message

#pre-conditions: control message received from ftserver over control connection,
#valid data port num
#post-conditions: will have checked if ftserver sent error message saying command
#was bad, return true if command good, false if command invalid
#description: accepts message from ftserver from control connection and a data port.
#checks if it matches the known error message. if matches, will print message
#describing invalid command and return false. otherwise command was valid, returns true
def RecdCommandCheck(controlMessage, dataPort):
	#the error message ftserver will send if the command sent by ftclient was invalid
	errorMessage = "Error, that command was invalid. Please use \"-l\" or \"-g <FILENAME>\"\n" 
	
	#check if this error message is in the message received from ftserver over the
	#control connection. If so, print this error message to the user and return false
	#since command was bad. otherwise if command was good, return true
	if(controlMessage == errorMessage):
		print(SERVER_HOST_ADDRESS + ":" + str(dataPort) + " says:\n" + controlMessage, end = '')
		return False
	return True

#pre-conditions: valid host address and port num for data connection
#post-conditions: will create socket, bind, and listen for connections, then return new socket descriptor
#description: accepts host address and port, creates socket, binds, then listens for new connections.
#returns socket descriptor.
def ServerSocketStartup(dataHostAddress, dataPort):
	#adapted from OSU CS 372 lecture 15 slides (specifically, slide 9)
	#and p.205 from Computer Networking-A Top-Down Approach by Kurose and Ross, 7th ed
	#create TCP server socket
	dataSocket = socket(AF_INET, SOCK_STREAM)

	#bind to the specified port num and address
	dataSocket.bind((dataHostAddress, dataPort))

	#listen for connections
	dataSocket.listen(1)

	#return the data connection server socket file descriptor
	return dataSocket

#pre-conditions: valid socket descriptor
#post-conditions: will accept a connection request from client
#description: accepts socket descriptor arg. calls accept() to 
#accept the request for connection from client. returns connectionSocket
#and addr info
def HandleRequest(socketFDData):
	#adapted from OSU CS 372 lecture 15 slides (specifically, slide 9)
	#and p.205 from Computer Networking-A Top-Down Approach by Kurose and Ross, 7th ed
	connectionSocket, addr = socketFDData.accept()

	return connectionSocket, addr

#pre-conditions: valid socket descriptor, prev initialized special delimiter
#post-conditions: will print the directory contents received and return the
#new connection socket and addr info.
#description: accepts socket descriptor and special delimiter that marks the
#end of the received transmission. calls handlerequest() to accept the connection.
#loops around getting message using recv() until the special EOF delimiter
#is in the message. if it's in the message, remove it and the EOF has been reached,
#then print the msg.if it's not in the message, print the msg and continue to loop.
#after entire msg received, return new socket descriptor and addr info.
def ReceiveDataDirectory(socketFDData, delimiter):
	#adapted from OSU CS 372 lecture 15 slides (specifically, slide 9)
	#accept connection
	connectionSocket, addr = HandleRequest(socketFDData)

	#create empty string
	message = ""

	#check for the special delimiter in the message indicating EOF. as long as delimiter
	#isn't in message, keep looping as this means EOF not yet reached.
	while(delimiter not in message):
		#receive the message from the socket
		message = connectionSocket.recv(MAX_MESSAGE_SIZE).decode()

		#if the delimiter was in message, EOF reached. remove delim and print message
		if(delimiter in message):
			#removing a char from a string adapted from:
			#https://www.journaldev.com/23674/python-remove-character-from-string
			messageNoDelim = (message.replace(delimiter, ''))
			print(messageNoDelim)
		#else just print message
		else:
			print(message, end = '')

	#return the connection socket FD and addr info
	return connectionSocket, addr

#pre-conditions: valid socket descriptor, valid special delimiter char, previously validated filename
#post-conditions: will have written received contents to a file
#description: accepts socket descriptor, special EOF delimiter, and a validated filename. uses handlerequest()
#to accept the connection. opens the file to be written to. loops around receiving data from the data
#connection using recv() for as long as the special EOF delimiter char isn't in the msg. if it's not in the
#message, keep looping and getting more data. if delim is in the message, will remove it and write that
#line to file. otherwise, can just write the message to the file without editing anything in the message.
#closes the file, returns new socket descriptor and addr info.
def ReceiveDataFile(socketFDData, delimiter, filename):
	#adapted from OSU CS 372 lecture 15 slides (specifically, slide 9)
	#accept connection
	connectionSocket, addr = HandleRequest(socketFDData)
 
	#create empty string
	message = ""

	#open a file w/ "w" option so can write to it
	#writing to a file with python adapted from:
	#https://www.digitalocean.com/community/tutorials/how-to-handle-plain-text-files-in-python-3
	file1 = open(filename, "w")

	#check for the special delimiter in the message indicating EOF. as long as delimiter
	#isn't in message, keep looping as this means EOF not yet reached.
	while(delimiter not in message):
		#receive the message from the socket
		message = connectionSocket.recv(MAX_MESSAGE_SIZE).decode()

		#if the delimiter was in message, EOF reached. remove delim and write message to file
		if(delimiter in message):
			#removing a char from a string adapted from:
			#https://www.journaldev.com/23674/python-remove-character-from-string
			messageNoDelim = (message.replace(delimiter, ''))
			file1.write(messageNoDelim)
		#else just write message to the file
		else:
			file1.write(message)
	
	#close the file
	file1.close()

	#return the connection socket FD and addr info
	return connectionSocket, addr

#pre-conditions: valid message received from ftserver over the control connection,
#valid data port num.
#post-conditions: will have validated the filename to see if ftserver was able to
#find a file with that name or not, returning true if ftserver didn't send error,
#false if ftserver sent error message.
#description: accepts message from ftserver over control connection and data port num.
#checks if the msg received matches the known error message. if does, prints error message
#and returns false since ftserver couldn't find that file name in its directory. otherwise,
#ftserver did find a file with that name and returns true
def FileNameFound(controlMessage, dataPort):
	#the error message ftserver will send if the file wasn't found
	errorMessage = "File not found.\n"

	#check if this error message is in the message received from ftserver over the
	#control connection. If so, print this error message to the user and return false
	#since file name wasn't found. Otherwise if file was found, return true.
	if(controlMessage == errorMessage):
		print(SERVER_HOST_ADDRESS + ":" + str(dataPort) + " says FILE NOT FOUND")
		return False
	else:
		return True

#pre-conditions: receives a filename
#post-conditions: will return bool indicating whether that file is present in the
#same directory as ftclient or not.
#description: accepts a filename. gets a list of current directory contents. loops
#through these contents, checking if any of them match the filename passed in (i.e.
#if that name is a duplicate and there's already a file in the current ftclient directory
#with that filename). if there is a duplicate, returns true. else returns false.
def DupFileCheck(filename):
	#getting everything from a directory in python adapted from:
	#https://stackoverflow.com/questions/3207219/how-do-i-list-all-files-of-a-directory
	#get contents of the directory in a list
	directoryContents = os.listdir()

	#loop through each item in the list
	for x in directoryContents:
		#if any of the list items is the same as the filename, filename is duplicate,
		#return true. otherwise return false.
		if(x == filename):
			return True;

	return False;

#pre-conditions: filename that has been already verified as a duplicate filename
#post-conditions: will have a new filename ready to be written to, whether it's the same
#duplicate name and user has chosen to overwrite, or whether it's a new name chosen by the
#user that's not a duplicate.
#description: receives filename of a duplicate file. prompts user for choice whether to overwrite
#or not. validates this userchoice. if yes, will overwrite the file with that name. if no, will
#ask for a new file name. if new file name given is also a duplicate, will loop around and prompt
#again for if they want to overwrite that filename or give a new one. if user doesn't enter either
#yes or no at the initial prompt, will print error message and loop back around. continues until
#user has either decided to overwrite a duplicate filename, or give a new filename. then either way,
#will return that filename to be used to write to.
def GetDupFileChoice(filename):
	#print message to user about duplicate file and prompt for choice to overwrite or rename
	print("\nThe filename (" + str(filename) + ") you entered is a duplicate. Do you want to overwrite it?")
	print("Type \"yes\" (minus quotes) to overwrite this file, or \"no\" (minus quotes) to enter a new filename: ", end = '')

	#create empty userChoice string and loop count to 0
	userChoice = ""
	count = 0

	#loop for as long as the user's choice wasn't yes or no
	while((str(userChoice) != "yes") and (str(userChoice) != "no")):
		#get user input. adapted from my own work for OSU CS 372 Project 1 Fall 2019
		userChoice = input()

		#if the user entered yes and this is the first loop, the new file name is same as old file name.
		#if not first time through loop, leave the newfilename as whatever they specified it to be as the new file name in
		#the elif statement below from a previous loop
		if(str(userChoice) == "yes"):
			if(count == 0):
				newFilename = filename
		#if the user said no, prompt for new file name and get it.
		elif(str(userChoice) == "no"):
			print("Enter new file name: ", end = '')
			newFilename = input()

			#check if the new file name entered is also a duplicate. If so, prompt again and set userChoice to "ERROR" so loop
			#will continue. increment count so if the user says yes to overwriting this new duplicate that file gets saved not
			#the original duplicate.
			if(DupFileCheck(newFilename) == True):
				print("\nYou entered another duplicate filename (" + str(newFilename) + "), please try again.")
				print("Type \"yes\" (minus quotes) to overwrite this file ("+ str(newFilename) + "), or \"no\" (minus quotes) to enter a new filename: ", end = '')
				userChoice = "ERROR"
				count = count + 1
		#if the user didn't enter either yes or no, prompt again and let the loop continue for new input
		else:
			print("\nYou didn't enter \"yes\" or \"no\", please try again.")
			print("Type \"yes\" (minus quotes) to overwrite this file, or \"no\" (minus quotes) to enter a new filename: ", end = '')
	
	#return the new filename
	return newFilename
						
#pre-conditions: user must have entered some command line args when they ran the python script
#post-conditions: all functions will have been called, sockets closed and cleaned up, any control
#and/or data information will have been sent/received via sockets with ftserver.
#description: main function. Calls other functions to check args, create variables and sockets, 
#handle control connection with ftserver as a client, handle data connection with ftserver as
#a server. once any control or data connection work is done, will cleanup/close the socket
#and terminate.
def main():
	#check num of args and validity of the command line args
	numArgs = ArgNumCheck()

	#using a non-printable ascii control character as a delimiter to separate messages
	#so that there's no chance of the delimiter being present in the command name, file
	#name, or file contents (ascii 3 is "end of text" control character)
	#list of ascii control characters found from:
	#https://www.ascii-code.com/
	delimiter = chr(3)

	#create bool for if the file requested in the command when running ftclient was present
	#on the host where ftserver is running or not and initialize to false
	isValidFileFTserver = False

	#(args must be either 5 or 6 because number of args was prev checked above)
	#if 6 command line args, assign the vars using six args function
	if numArgs == MAX_NUM_ARGS:
		controlPort, serverHost, command, filename, dataPort, controlMessage = SixArgAssignVars(delimiter)	
	#if 5 command line args, assign the vars using the five args function
	else:
		controlPort, serverHost, command, dataPort, controlMessage = FiveArgAssignVars(delimiter)

	#create listening socket for data connection to be ready for ftserver connections.
	#due to slower python interpretation time compared to execution of c++ program, another student
	#on OSU CS 372 slack (Andrew Freitas) recommended starting the listening early in ftclient
	socketFDData = ServerSocketStartup(CLIENT_HOST_ADDRESS, int(dataPort))

	#initiate contact with ftserver for the control connection
	socketFDControl = InitiateContact(int(controlPort), serverHost)

	#send control connection message to ftserver
	MakeRequest(socketFDControl, controlMessage)

	#receive control connection response from ftserver
	controlMessage = ReceiveDataControl(socketFDControl)

	#check if ftserver found the command send in the controlMessage to be valid or not
	isValidCommand = RecdCommandCheck(controlMessage, dataPort)

	#if the command was valid and it was -g and a filename (instead of just -l),
	#check if ftserver found the filename ftclient requested on the host ftserver is
	#running on.
	if ((isValidCommand == True) and (command == GET_COMMAND)): 
		isValidFileFTserver = FileNameFound(controlMessage, dataPort)

	#if was a valid command (either -l or -g and a filename), continue
	if isValidCommand == True:
		#if command was "-l", receive the directory contents sent from ftserver
		if(command == LIST_COMMAND):
			print("Receiving directory structure from " + SERVER_HOST_ADDRESS + ":" + str(dataPort))
			
			#accept connection and receive directory contents data from ftserver and print
			connectionSocket, addr = ReceiveDataDirectory(socketFDData, delimiter)

			#close data connection socket
			connectionSocket.close()
		#if the filename is found on the flip ftserver is running on and the command was -g, receive
		#file contents from ftserver
		elif((isValidFileFTserver == True) and (command == GET_COMMAND)):
			#check if the filename for the file we'll write the contents from ftserver to is a duplicate
			#for a file in the directory on the host where ftclient is running
			dupFileFound = DupFileCheck(filename)
			
			#if the filename for the file we'll write the contents from ftserver to is a duplicate filename
			#for a file in the directory on the host where ftclient is running, handle this by giving the
			#user a choice to overwrite or rename
			if(dupFileFound == True):
				newFilename = GetDupFileChoice(filename) 
			#if there was no duplicate filename, just set the newfile name to the original filename
			else:
				newFilename = filename
				
			print("Receiving \"" + filename + "\" from " + SERVER_HOST_ADDRESS + ":" + str(dataPort))
			print("Writing to \"" + newFilename + "\" from " + SERVER_HOST_ADDRESS + ":" + str(dataPort))

			#receive message over the data connection and write to the new file
			connectionSocket, addr = ReceiveDataFile(socketFDData, delimiter, newFilename)
			print("File transfer complete.")
			#close data connection socket
			connectionSocket.close()

	#close the control socket
	socketFDControl.close()

#used to call the main function
#using a main() function in python excerpted from:
#https://www.journaldev.com/17752/python-main-function
if __name__ == '__main__':
	main()
