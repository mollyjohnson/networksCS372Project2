#!/usr/bin/env python3

#although this file doesn't have the .py extension, it is written in python and the above shebang was included
#to make sure it will be run using python3 on the flip server.

#Programmer Name: Molly Johnson (johnsmol)
#Program Name: chatserve 
#Program Description: chatserve is the server portion of this chat program. A socket will be set up
#and started, waiting to accept connections on a specified hostname and portnumber. It will then
#alternate sending and receiving messages back and forth with the client program until a SIGINT (ctrl-c)
#signal is received, at which point chatserve will terminate.
#Course Name: CS 372 Intro to Networks
#Last Modified: 11/04/19

#imported libraries for socket (to create socket and use it) and sys (system, used to get input from the user)
from socket import *
import sys

#pre-conditions: none
#post-conditions: number of args will have been checked and the validity of the port argument will have been checked
#description: this function accepts no arguments and returns nothing. Will first check if the correct number of arguments
#has been entered (2, one for the program and one for the port number), then if the number of args is correct the port number
#arg will be checked to make sure it's a non-negative integer, and then if it's a non-negative integer it will be checked to
#make sure that it's not a number above the maximum valid port number possible. If any of these are true (wrong num of args
#entered, negative integer or a string were entered as the port number, or a port number above the allowable range was entered),
#the program will print an error message to the user and exit.
def ArgCheck():
	#should be 2 args, one for program name and one for port number
	if len(sys.argv) != 2:
		print('Wrong number of arguments! Must enter a valid port number. Start the program again.')
		exit()
	#else if there's the right nuber of arguments, make sure the argument is a valid port number.
	#using isdigit() to check if a string represents a non-negative integer adapted from:
	#https://stackoverflow.com/questions/1265665/how-can-i-check-if-a-string-represents-an-int-without-using-try-except
	if sys.argv[1].isdigit() == False:
		print('You entered a negative integer or string for the port number! Must enter a valid port number. Start the program again.')
		exit()
	#if a non-neg integer was entered, check it's in valid port num range. valid port number range: 0-65535
	#valid port values excerpted from:
	#https://www.webopedia.com/quick_ref/portnumbers.asp
	if int(sys.argv[1]) > 65535:
		print('You entered a port number above the valid port number range. Must enter a valid port number. Start the program again.')
		exit()

#pre-conditions: maxMessageSize must be known (in this case we know the max message size is 500 from the assignment instructions, and I
#added one character for the null terminator). Although this parameter isn't passed in as an argument and can instead be defined in the
#function, the max message size must be known ahead of time. serverHandle argument must be a valid string.
#post-conditions: valid input will have been obtained from the user and this input message will be returned.
#description: this function accepts one argument and returns a string message that has been input by the user. Will check that this message
#is valid and if not valid keep looping with an error message and request for new input from the user until a valid message of 500 chars
#or less has been obtained. This validated message will then be returned.
def GetMessage(serverHandle, maxMsgPlusHandle):
	#start validMessage bool to false
	validMessage = False

	#continue to loop and get user input as long as the input is invalid (i.e. validMessage is false)
	while(validMessage == False):
		#get input from the user
		message = input()

		#if the message is below the max size, set validMessage bool to true so breaks out of loop since valid input has been received
		if(len(message) <= maxMsgPlusHandle):
			validMessage = True
		#if the message is above the max message size, print an error message to user and reprint handle, so chatserve user can enter
		#a new message
		else:
			print("Message too long. Try again.")
			print(serverHandle + '> ', end = '')

	#return validated message
	return message

#pre-conditions: valid host address and valid port number
#post-conditions: socket will be created, bound, and listening, with the socket being returned
#description: This function will accept two arguments, a valid host address (flip1 in this case)
#and a valid port number. It will then use the SOCKET API to create a server socket (and SOCK_STREAM
#to make this a TCP socket), bind that socket to the host and port, and begin listening for TCP
#connection requests. It will then return the server socket.
def SocketStartup(hostAddress, serverPort):
	#socket server setup excerpted from OSU CS 372 lecture 15 slides (specifically, slide 9)
	#and p.205 from Computer Networking-A Top-Down Approach by Kurose and Ross, 7th ed
	#create socket for server (SOCK_STREAM indicates this is a TCP connection)
	serverSocket = socket(AF_INET, SOCK_STREAM)

	#bind the socket to the specified host address and server port
	serverSocket.bind((hostAddress, serverPort))

	#have server start listening for TCP connection requests from clients
	serverSocket.listen(1)

	return serverSocket

#pre-conditions: already created server socket, maximum message size + handle must be known
#post-conditions: will return the connection socket, address, and message received from
#the client.
#description: This function will accept two arguments, a server socket and the max size
#of the server handle plus the max size of the client message. Will use accept() to wait
#for connection requests. Will then user recv() to get the bytes from the client over the
#socket connection. Will use .decoe() to decode those bytes back into a string. Will then
#return the connection socket and address returned from .accept() as well as the message
#received from the client (their prepended handle plus message).
def ReceiveMessage(serverSocket, maxMsgPlusHandle):
	#adapted from OSU CS 372 lecture 15 slides (specifically, slide 9)
	#and p.205 from Computer Networking-A Top-Down Approach by Kurose and Ross, 7th ed
	#have server wait for requests
	connectionSocket, addr = serverSocket.accept()
	#receive the prepended handle + message from the client
	clientMessage = connectionSocket.recv(maxMsgPlusHandle).decode()

	return connectionSocket, addr, clientMessage

#pre-conditions: a valid string message and known phrase that will cause the program to quit
#post-conditions: will determine if the message consists of user handle (client or server) +
#"> " and the phrase that will cause the program to quit.
#description: This function accepts two arguments, a string message and a "quit" phrase. It will
#loop through the number of chars in the quit phrase backward until the beginning of the quit phrase,
#and compare each char in the message to that of the quit phrase, space, and arrow that comes after
#the client's or server's handle. In this way, the "\quit" message will only be taken as a command
#to quit the client connection when entered as "\quit" alone by the user, and not part of a longer
#message, like "when you want to quit just type \quit". Will then compare the number of matches of
#those characters to the number of chars in the quit phrase. If they're equal the message was just "\quit".
#If not, the client or server message was not "\quit". If the message from the client or server was
#"\quit", will return true. If not, will return false. 
def QuitCheck(message, quitPhrase):
	#use > \quit as phrase plus delimiter
	quitPhrasePlusDelimiter = ("> " + quitPhrase)
	#start isQuit to false
	isQuit = False
	#start quitcount at 0 so can be incremented
	quitCount = 0
	#have k be last index in message string
	k = (len(message) - 1) 
	#have j be last index in quitphrase string
	j = (len(quitPhrasePlusDelimiter) - 1)
	#start loop counter at 0
	loopCount = 0
	#have the number of chars needed for a quit phrase match to be the length of the quit phrase + delimiter
	phraseMatchCount = len(quitPhrasePlusDelimiter)	

	#if the message isn't as long as the quit phrase, don't check it as the user can't have entered \quit
	#and this would cause a crash by trying to compare index values not there.
	if(len(message) >= phraseMatchCount):
		#loop for the length of the quit phrase
		while (loopCount < phraseMatchCount):
			#compare each char in quit phrase + delimiter to that of the same index in the message
			if(message[k] == quitPhrasePlusDelimiter[j]):
				#if they matched, increment quit count
				quitCount = quitCount + 1

			#eecrement the indexes and increment the loop counter
			k = k - 1
			j = j - 1
			loopCount = loopCount + 1

	#if the number of matching chars from the message is equal to the length of the quitphrase, the user entered
	#"\quit", set isQuit to true. Otherwise let it stay false
	if(quitCount == phraseMatchCount):
		isQuit = True

	return isQuit

#pre-conditions: valid server handle, valid server message, valid connection socket
#post-conditions: will send the server's handle, "> ", and actual message to the client
#description: Will accept three arguments, handle, message, and connection socket. Will use
#the SOCKET .send() function to send this message to the client and .encode() to turn the
#string into bytes.
def SendMessage(serverHandle, serverMessage, connectionSocket):
	#adapted from OSU CS 372 lecture 15 slides (specifically, slide 9)
	#and p.205 from Computer Networking-A Top-Down Approach by Kurose and Ross, 7th ed
	connectionSocket.send((serverHandle + '> ' + serverMessage).encode())

#pre-conditions: none, this is the main function of the program
#post-conditions: socket will have been set up, messages received and sent w a client when one connects, and after the connection has been closed
#with that client via the "\quit" command, the server will remain waiting on the host and port number indicated, waiting for any additional client
#connections, until a SIGINT (ctrl-c) is received.
#description: the main function of the program. Calls other functions to perform actions like getting user input, creating the socket, sending
#and receiving data with the client, etc. See each functions for details. will close connection when user enters \quit and terminate when receives a SIGINT.
def main():
	#check user entered correct port number argument
	ArgCheck()

	#convert system arg 1 (string) to an int for the server port number
	#getting command line arguments using sys library and argv adapted from:
	#https://stackoverflow.com/questions/4033723/how-do-i-access-command-line-arguments-in-python
	#converting a string to an int adapted from:
	#https://www.digitalocean.com/community/tutorials/how-to-convert-data-types-in-python-3
	serverPort = int(sys.argv[1]) 
	
	#specify host address to a flip server (since not specified in the command line per the assignment instructions and rubric, command line args
	# should just be program name and port number for the server)
	hostAddress = 'flip1.engr.oregonstate.edu'

	#hard-coded server handle (okay per the assignment instructions)
	serverHandle = "kermit"

	#phrase used to indicate you want to quit the program
	quitPhrase = "\\quit"

	#max message is 501 (500 + 1 for null term), max handle from client is 11 (10 + 1 for null term),
	#gave one extra for handle plus message null term.
	maxMsgPlusHandle = 513
	
	#perform socket startup
	serverSocket = SocketStartup(hostAddress, serverPort)

	#continue looping until you get a SIGINT (ctrl-c)
	while True:
		#receive message from client
		connectionSocket, addr, clientMessage = ReceiveMessage(serverSocket, maxMsgPlusHandle)

		#check if the client entered "\quit"
		if (QuitCheck(clientMessage, quitPhrase) == True):
			print(clientMessage)
			print("the client is disconnecting.")
			
			#close the socket
			#excerpted from OSU CS 372 lecture 15 slides (specifically, slide 9)
			#and p.205 from Computer Networking-A Top-Down Approach by Kurose and Ross, 7th ed
			connectionSocket.close()

		#if the client didn't request to quit, print their handle + message,
		#then print the server's handle and call GetMessage to get chatserve user input.
		else:
			print(clientMessage)

			#using end='' to prevent the newline from being printed with the print() function adapted from:
			#https://www.stechies.com/python-print-without-newline/
			print(serverHandle + '> ', end = '')

			#call GetMessage to get the user's input for message to send to client
			serverMessage = GetMessage(serverHandle, maxMsgPlusHandle)	

			#check if the server user entered "\quit"
			if (QuitCheck((serverHandle + "> " + serverMessage), quitPhrase) == True):
				print("the client is disconnecting.")

			#send the prepended chatserve handle plus the chatserve user's message to the client
			SendMessage(serverHandle, serverMessage, connectionSocket)

			#close the socket
			#excerpted from OSU CS 372 lecture 15 slides (specifically, slide 9)
			#and p.205 from Computer Networking-A Top-Down Approach by Kurose and Ross, 7th ed
			connectionSocket.close()


#used to call the main function
#using a main() function in python excerpted from:
#https://www.journaldev.com/17752/python-main-function
if __name__ == '__main__':	
	main()