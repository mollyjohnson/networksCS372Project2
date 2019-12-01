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

MAX_NUM_ARGS = 6
MIN_NUM_ARGS = 5
MAX_PORT = 65535
MIN_PORT = 1025
LIST_COMMAND = "-l"
GET_COMMAND = "-g"
SERVER_HOST_ADDRESS = "flip1.engr.oregonstate.edu"
CLIENT_HOST_ADDRESS = "flip2.engr.oregonstate.edu"
#MAX_MESSAGE_SIZE = 100
MAX_MESSAGE_SIZE = 1000

#pre-conditions:
#post-conditions:
#description:
def FiveArgAssignVars(delimiter):
	controlPort = int(sys.argv[2])
	serverHost = sys.argv[1]
	command = sys.argv[3]
	dataPort = int(sys.argv[4])
	controlMessage = (command + delimiter + str(dataPort) + delimiter)
	return controlPort, serverHost, command, dataPort, controlMessage

#pre-conditions:
#post-conditions:
#description:
def SixArgAssignVars(delimiter):
	controlPort = int(sys.argv[2])
	serverHost = sys.argv[1]
	command = sys.argv[3]
	filename = sys.argv[4]
	dataPort = int(sys.argv[5])
	controlMessage = (command + delimiter + filename + delimiter + str(dataPort) + delimiter)
	return controlPort, serverHost, command, filename, dataPort, controlMessage

#pre-conditions:
#post-conditions:
#description:
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
	#if ((int(sys.argv[2])) == (int(sys.argv[4]))):
	#	print("You entered the same port number for the control port and data port. This is not allowed. Start the program again.")
	#	exit()

#pre-conditions:
#post-conditions:
#description:
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
	#if ((int(sys.argv[2])) == (int(sys.argv[5]))):
	#	print("You entered the same port number for the control port and data port. This is not allowed. Start the program again.")
	#	exit()

#pre-conditions:
#post-conditions:
#description:
def ArgNumCheck():
	if ((len(sys.argv) != MAX_NUM_ARGS) and (len(sys.argv) != MIN_NUM_ARGS)):
		print("Wrong number of arguments! Must enter server host, server port num, commmand, filename (optional), and data port num. Start the program again.")
		exit()
	if(sys.argv[1] != SERVER_HOST_ADDRESS):
		print("You didn't enter the correct server host address. Make sure to use " + SERVER_HOST_ADDRESS + " for the server host and start the program again.")
		exit()
	if (len(sys.argv) == MAX_NUM_ARGS):
		SixArgCheck()
	else:
		FiveArgCheck()
	return len(sys.argv)

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
def SendMessage(sockFD, message):
	#adapted from OSU CS 372 lecture 15 slides (specifically, slide 9)
	#and p.205 from Computer Networking-A Top-Down Approach by Kurose and Ross, 7th ed
	sockFD.send((message).encode())

#pre-conditions:
#post-conditions:
#description:
def ReceiveMessage(sockFD):
	message = sockFD.recv(MAX_MESSAGE_SIZE).decode()
	return message

#pre-conditions:
#post-conditions:
#description:
def RecdCommandCheck(controlMessage):
	errorMessage = "Error, that command was invalid. Please use \"-l\" or \"-g <FILENAME>\"\n" 
	if(controlMessage == errorMessage):
		print(controlMessage, end = '')
		return False
	return True

#pre-conditions:
#post-conditions:
#description:
def SocketStartup(dataHostAddress, dataPort):
	dataSocket = socket(AF_INET, SOCK_STREAM)
	dataSocket.bind((dataHostAddress, dataPort))
	dataSocket.listen(1)

	return dataSocket

def ReceiveMessageDirectory(socketFDData, delimiter):
	connectionSocket, addr = socketFDData.accept()

	#creating a dynamic array in python adapted from:
	#https://stackoverflow.com/questions/2910864/in-python-how-can-i-declare-a-dynamic-array
	dataArray = []
	message = ""
	while(delimiter not in message):
		message = connectionSocket.recv(MAX_MESSAGE_SIZE).decode()
		if(delimiter in message):
			#removing a char from a string adapted from:
			#https://www.journaldev.com/23674/python-remove-character-from-string
			messageNoDelim = (message.replace(delimiter, ''))
			#dataArray.append(messageNoDelim)
			print(messageNoDelim)
		else:
			#dataArray.append(message)
			print(message, end = '')
	return connectionSocket, addr, dataArray

def ReceiveMessageFile(socketFDData, delimiter, filename):
	connectionSocket, addr = socketFDData.accept()

	#creating a dynamic array in python adapted from:
	#https://stackoverflow.com/questions/2910864/in-python-how-can-i-declare-a-dynamic-array
	dataArray = []
	message = ""
	TEMP_FILENAME = "newFile.txt"
	file1 = open(TEMP_FILENAME, "w")
	while(delimiter not in message):
		message = connectionSocket.recv(MAX_MESSAGE_SIZE).decode()
		if(delimiter in message):
			#removing a char from a string adapted from:
			#https://www.journaldev.com/23674/python-remove-character-from-string
			messageNoDelim = (message.replace(delimiter, ''))
			#dataArray.append(messageNoDelim)
			#print(messageNoDelim)
			file1.write(messageNoDelim)
		else:
			#dataArray.append(message)
			#print(message, end = '')
			file1.write(message, end = '')
	file1.close()
	return connectionSocket, addr, dataArray

#pre-conditions:
#post-conditions:
#description:
def FileNameFound(controlMessage):
	print("the control message is:" + controlMessage)
	errorMessage = "File not found.\n"
	if(controlMessage == errorMessage):
		print(controlMessage, end = '')
		return False
	else:
		#print("file was found.\n")
		return True
#pre-conditions:
#post-conditions:
#description:	
def GetDupFileChoice():
	#handle duplicate file
	print("The filename you entered is a duplicate. Do you want to overwrite it?")
	print("Type \"yes\" (minus quotes) to overwrite this file, or \"no\" (minus quotes) to enter a new filename.")
	userChoice = ""
	while((str(userChoice) != "yes") and (str(userChoice) != "no")):
		userChoice = input()
		if(str(userChoice) == "yes"):
			newFilename = filename
		elif(str(userChoice) == "no"):
			print("Enter new file name:")
			newFilename = input()
			if(newFilename == filename):
				print("You entered the same duplicate filename, please try again.")
				print("Enter new file name:")
				userChoice = "ERROR"
		else:
			print("You didn't enter \"yes\" or \"no\", please try again.")
			print("Type \"yes\" (minus quotes) to overwrite this file, or \"no\" (minus quotes) to enter a new filename.")
return newFilename
						
#pre-conditions:
#post-conditions:
#description:
def main():
	#check num and validity of the command line args
	numArgs = ArgNumCheck()

	#using a non-printable ascii control character as a delimiter to separate messages
	#so that there's no chance of the delimiter being present in the command name, file
	#name, or file contents (ascii 3 is "end of text" control character)
	#list of ascii control characters found from:
	#https://www.ascii-code.com/
	delimiter = chr(3)
	isValidFileFTserver = False

	if numArgs == MAX_NUM_ARGS:
		controlPort, serverHost, command, filename, dataPort, controlMessage = SixArgAssignVars(delimiter)	
	else:
		controlPort, serverHost, command, dataPort, controlMessage = FiveArgAssignVars(delimiter)

	#create listening socket for data connection
	socketFDData = SocketStartup(CLIENT_HOST_ADDRESS, int(dataPort))

	socketFDControl = InitiateContact(int(controlPort), serverHost)

	SendMessage(socketFDControl, controlMessage)
	controlMessage = ReceiveMessage(socketFDControl)
	isValidCommand = RecdCommandCheck(controlMessage)

	if ((isValidCommand == True) and (command == GET_COMMAND)): 
		isValidFileFTserver = FileNameFound(controlMessage)

	#check if the command was valid
	if isValidCommand == True:
		#if command was "-l", receive the directory contents sent from ftserver
		if(command == LIST_COMMAND):
			print("in the -l bit, need to receive the list of directory contents")
			#accept connection and receive directory contents data from ftserver
			connectionSocket, addr, directoryContents = ReceiveMessageDirectory(socketFDData, delimiter)

			#print each item in the dataMessage dynamic array
			#for <item> in <array> loop use adapted from:
			#https://stackoverflow.com/questions/2910864/in-python-how-can-i-declare-a-dynamic-array
			#for object in directoryContents:
			#	print(object)
			#close data connection socket
			connectionSocket.close()
		#if the filename is found on the flip ftserver is running on and the command was -g
		elif((isValidFileFTserver == True) and (command == GET_COMMAND)):
			#dupFileFound = DupFileCheck(filename)
			dupFileFound = False
			
			print("the duplicate file bool is: " + str(dupFileFound))
			
			#check if the filename already exists on the flip ftclient is running on
			if(dupFileFound == true):
				newFilename = GetDupFileChoice() 
				
				connectionSocket, addr, fileContents = ReceiveMessageFile(socketFDData, delimiter, newFilename)
			#else if the filename doesn't exist on the flip ftclient is running on
			else:
				connectionSocket, addr, fileContents = ReceiveMessageFile(socketFDData, delimiter, filename)	
			#print each item in the dataMessage dynamic array
			#for <item> in <array> loop use adapted from:
			#https://stackoverflow.com/questions/2910864/in-python-how-can-i-declare-a-dynamic-array
			#for object in fileContents:
			#	print(object)
			#close data connection socket
			connectionSocket.close()

	#close the control socket
	socketFDControl.close()

#used to call the main function
#using a main() function in python excerpted from:
#https://www.journaldev.com/17752/python-main-function
if __name__ == '__main__':
	main()
