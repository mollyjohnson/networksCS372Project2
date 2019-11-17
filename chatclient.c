/*
Programmer Name: Molly Johnson (johnsmol)
Program Name: chatclient
Program Description: chatclient is the client portion of this chat program. A socket will be
created and a connection initiated, connecting to the server on a specified hostname and portnumber.
It will then alternate sending and receiving messages back and forth with the server until either the
client or the server sends a "\quit" message, at which point chatclient will terminate.
Course Name: CS 372 Intro to Networks
Last Modified: 11/04/19
*/

//include other header files
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <arpa/inet.h>

//macro definitions
//message sizes, handle sizes, and quit phrase determined from assignment instructions. Host address
//for the server described in README.txt. TRUE and FALSE set since using raw c and raw c doesn't have bools.
#define MAX_MESSAGE_SIZE 501
#define MAX_MSG_PLUS_HANDLE 513
#define HOST_ADDRESS "flip1.engr.oregonstate.edu"
#define MAX_HANDLE_SIZE 11
#define TRUE 1
#define FALSE 0
#define QUIT "\\quit" 
#define QUIT_PLUS_DELIMITER "> \\quit"

/*
pre-condititwo two valid c strings
post-conditions: a bool will be returned, true if the strings matched and false if the strings didn't match
description: this function accepts two strings as arguments. It then uses strcmp to compare the strings
and returns true if the strings matched, false if the strings didn't match. (I made this function because I could
never remember if strcmp should return 0 for matching or not matching so felt this made it more explicit). This
function is adapted from my own work created for OSU CS 344 Winter 2019 assignment 3, last updated 3-3-19.
*/
int StringMatch(char *string1, char *string2){
	//use strcmp to see if string one and two are equal. if equal, strcmp will return 0.
	//strcmp will return non-zero if the two strings are not equal.
	if(strcmp(string1, string2) == 0){
		//if the two strings are a match, return true
		return TRUE;
	}
	//if the two strings are not a match, return false
	return FALSE;
}

/*
pre-conditions: a valid c string
post-conditions: will return a bool, true if the string was just a newline char, false if there were
other characters in the string. 
description: will accept one argument, a string. Will check if that string is just a newline character.
If it is, will return true. otherwise will return false. This function is adapted from my own work created
for OSU CS 344 Winter 2019 assignment 3, last updated 3-3-19. 
*/
int IsNewline(char *userInputIn){
	//call StringMatch to check if the user input string is just a newline character.
	if(StringMatch(userInputIn, "\n") == TRUE){
		//if it is, return true
		return TRUE;
	}
	//else, return false
	return FALSE;	
}

/*
pre-conditions: valid c string
post-conditions: will return a bool, true if the characters in the string are all integers, false otherwise.
description: will accept one argument, a string. Will use ascii values to check if each character in the string
is an integer value (i.e. would it work with atoi() to convert them to ints later). If all chars are for integer
ascii values, will return true. otherwise will return false. This function is adapted from my own work created
for OSU CS 344 Winter 2019 assignment 3, last updated 3-3-19. 
*/  
int IntInputValidation(char *inputString){
	//create variables for char to be checked, loop counter, and length of the input string
	char asciiValue;
	int inputLength = strlen(inputString);
	int i;

	//loop through entire string so can check each char
	for(i = 0; i < inputLength; i++){
		//set temp char equal to the current string char
		asciiValue = inputString[i];

		//check if the current char is within the valid ascii values for ints 0-9 (i.e. ascii values 48-57)
		if((asciiValue < 48) || (asciiValue > 57)){
			//if ascii value of char being checked is outside of the range for integers, return false
			return FALSE;
		}
	}
	//if all of the chars in the string to be checked were valid integers, return true
	return TRUE;
}

/*
pre-conditions: two valid strings passed in
post-conditions: will set the message to be the handle + user message, and return a bool, true if input was
valid, false if input was not valid
description: will accept two string arguments. will get user input, check it for validity, remove the newline char,
set the user message, and set the bool determining if the message was valid or not. Adapted from my own work 
created for OSU CS 344 Winter 2019 assignment 3, last updated 3-3-19 and from https://c-for-dummies.com/blog/?p=1112
*/
int GetMessage(char *userMessageIn, char *userHandleIn){
	//create buffer of size max
	char *buffer;
	size_t bufsize = MAX_MSG_PLUS_HANDLE;
	size_t characters;
	int goodInput = TRUE;

	//malloc the buffer and check that malloc is successful
	buffer = (char *)malloc(bufsize * sizeof(char));

	//if buffer == NULL, malloc had an error. print error message and exit with value of 1.
	if(buffer == NULL){
		perror("GETLINE BUFFER ERROR, UNABLE TO ALLOCATE\n");
		exit(1);
	}

	//keep looping (by using while(1)) to get the line of user input. check for if getline returns -1
	//is used to make sure getline doesn't encounter any problems due to signals
	while(1){
		//call getline to get user input from stdin and put it in the buffer
		characters = getline(&buffer, &bufsize, stdin);

		//check if getline returned -1
		if(characters == -1){
			//if getline returned -1, use clearerr on stdin and let it loop back around
			clearerr(stdin);
		}
		else{
			//else if getline was successful (didn't return -1), go ahead and break out of loop
			break;
		}
	}

	//check if buffer only contains newline
	if(IsNewline(buffer) == FALSE){
		//if buffer too big, print error message and set bool false
		if(strlen(buffer) > MAX_MESSAGE_SIZE){
			strcpy(userMessageIn, "Error");
			printf("Error, your message was too long. Please try again.\n");
			goodInput = FALSE;
		}
		else{
			//user entered a valid message, remove the newline char from the buffer, replacing it
			//with a null terminating character
			buffer[strcspn(buffer, "\n")] = '\0';

			//cat the buffer with the newline char removed into the userHandle string variable
			strcpy(userMessageIn, userHandleIn);
			strcat(userMessageIn, "> ");
			strcat(userMessageIn, buffer);
		}
	}
	else{
		//if user only entered newline char ie only hit enter, don't remove it and set it to message. set bool to true
		strcpy(userMessageIn, userHandleIn);
		strcat(userMessageIn, "> ");
		goodInput = TRUE;
	}
	
	//free the buffer that was malloc'd for getline and set to NULL
	free(buffer);
	buffer = NULL;

	//return bool for whether the input was good or not
	return goodInput;
}

/*
pre-conditions: valid non neg integer and valid command line args
post-conditions: will check if the arg count is right, args are valid, but returns nothing 
description: will take in num of args and the args from command line, check that they match
program requirements for num of args and types of args. will print error message and exit if invalid.
*/
void ArgCheck(int argCount, char *args[]){
	//arg count must be 3 to be valid
    if (argCount != 3){
        fprintf(stderr, "Wrong number of arguments! Must enter the correct hostname and a valid port number. Start the program again.\n");
        fflush(stdout); exit(1);    
    }
	//using atoi to convert from string to int adapted from:
	//https://www.quora.com/How-do-I-convert-character-value-to-integer-value-in-c-language
	//check if args are valid integers (and non negative) and aren't above valid port nums
	if ((!IntInputValidation(args[2])) || (atoi(args[2]) < 0 ) || (atoi(args[2]) > 65535)){
		fprintf(stderr, "You entered a negative number, string or otherwise invalid port number argument. Start the program again.\n");
       	fflush(stdout); exit(1);    
	}
	//make sure host address entered matches that of the server
	if (!StringMatch(args[1], HOST_ADDRESS)){
		fprintf(stderr, "Your host argument doesn't match the server host. Start the program again and use flip1.engr.oregonstate.edu as your host.\n");
       	fflush(stdout); exit(1);    
	}
}

/*
pre-conditions: valid string passed in 
post-conditions: will set the handle, and return a bool, true if input was valid, false if input was not valid
description: will accept string arg. will get user input, check it for validity, remove the newline char,
set the user handle, and set the bool determining if the was valid or not. Adapted from my own work 
created for OSU CS 344 Winter 2019 assignment 3, last updated 3-3-19 and from https://c-for-dummies.com/blog/?p=1112
*/
int GetHandle(char *userHandleIn){
	//create buffer of size MAX char size for handle
	char *buffer;
	size_t bufsize = MAX_HANDLE_SIZE;
	size_t characters;
	int goodInput = TRUE;

	//malloc the buffer and check that malloc is successful
	buffer = (char *)malloc(bufsize * sizeof(char));

	//if buffer == NULL, malloc had an error. print error message and exit with value of 1.
	if(buffer == NULL){
		perror("GETLINE BUFFER ERROR, UNABLE TO ALLOCATE\n");
		exit(1);
	}

	//keep looping (by using while(1)) to get the line of user input. check for if getline returns -1
	//is used to make sure getline doesn't encounter any problems due to signals
	while(1){
		//call getline to get user input from stdin and put it in the buffer
		characters = getline(&buffer, &bufsize, stdin);

		//check if getline returned -1
		if(characters == -1){
			//if getline returned -1, use clearerr on stdin and let it loop back around
			clearerr(stdin);
		}
		else{
			//else if getline was successful (didn't return -1), go ahead and break out of loop
			break;
		}
	}

	//check if string is only a newline buffer
	if(IsNewline(buffer) == FALSE){
		//check if the size is too big, if so set bool to false
		if(strlen(buffer) > MAX_HANDLE_SIZE){
			strcpy(userHandleIn, "Error");
			printf("Error, your handle was too long. Please try again with 10 characters or less.\n");
			goodInput = FALSE;
		}
		else{
		//user entered a valid handle, remove the newline char from the buffer, replacing it
		//with a null terminating character
		buffer[strcspn(buffer, "\n")] = '\0';

		//copy the buffer with the newline char removed into the userHandle string variable
		strcpy(userHandleIn, buffer);
		}
	}
	//if user only entered newline, print error and set bool to false
	else{
		strcpy(userHandleIn, "Error");
		printf("Error, you didn't enter a handle, you only hit enter. Please try again.\n");
		goodInput = FALSE;
	}
	
	//free the buffer that was malloc'd for getline and set to NULL
	free(buffer);
	buffer = NULL;

	//return bool for input validity
	return goodInput;
}

/*
pre-conditions: valid socket, valid string
post-conditions: will send string message to server
description: accepts socket file descriptor and string. will use send() to send that string
(as bytes) to the server. send() use adapted from: http://beej.us/guide/bgnet/html/#a-simple-stream-client
*/
void SendMessage(int sockFD, char *sendBuf){
	int charsW;
	//send message to server
	charsW = send(sockFD, sendBuf, strlen(sendBuf), 0);
	//check that chars written is >0
	if (charsW < 0){
		fprintf(stderr, "Error writing to socket.\n"); fflush(stdout); exit(1);
	}
	//check that chars written is >= the size of the string
	if (charsW < strlen(sendBuf)){
		fprintf(stderr, "Warning: some, but not all data written to socket.\n"); fflush(stdout); exit(1);
	}

	//use ioctl to check that all chars were sent. adapted from my own work in osu cs 344 winter 2019, created 3/3/19
	int checkSend = -5;
	do{
		ioctl(sockFD, TIOCOUTQ, &checkSend);
	} while(checkSend > 0);

	//if checksend is <0, was an error
	if (checkSend < 0){
		fprintf(stderr, "Ioctl error.\n"); fflush(stdout); exit(1);
	}
}

/*
pre-conditions: valid socket, valid string
post-conditions: will receive message from server
description: accepts two arguments, socket file descriptor and string. uses recv() to receive
bytes from server. recv() use adapted from: http://beej.us/guide/bgnet/html/#a-simple-stream-client
*/
void ReceiveMessage(int sockFD, char *recvBuf){
	int charsR;
	//receive message of size max num of message chars plus max num handle chars minus 1
	charsR = recv(sockFD, recvBuf, (MAX_MSG_PLUS_HANDLE - 1), 0);
	//check if the num of chars received is <0
	if (charsR < 0){
		fprintf(stderr, "Error reading from the socket.\n"); fflush(stdout); exit(1);
	}
}

/*
pre-conditions: valid addrinfo struct
post-conditions: returns socket file descriptor
description: accepts addrinfo struct arg. uses socket() to create socket and connect() to
connect it to the server. returns socket file descriptor socket() and connect() use adapted from:
http://beej.us/guide/bgnet/html/#a-simple-stream-client
*/
int InitiateContact(struct addrinfo *servinfo){
	//create socket w servinfo
	int sockFD = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	//if socket file descriptor <0, was an error
	if (sockFD < 0){
		fprintf(stderr, "Error creating socket descriptor.\n"); fflush(stdout); exit(1);
	}	
	//connect socket to the server
	int statusConnect = connect(sockFD, servinfo->ai_addr, servinfo->ai_addrlen);
	//if connection status <0, was an error connecting
	if (statusConnect < 0){
		fprintf(stderr, "Error connecting to server.\n"); fflush(stdout); exit(1);
	}
	return sockFD;
}

/*
pre-conditions: a valid string message
post-conditions: will determine if the message consists of user handle (client or server) +
"> " and the phrase that will cause the program to quit.
description: This function accepts one argument, a string message. It will
loop through the number of chars in the quit phrase backward until the beginning of the quit phrase,
and compare each char in the message to that of the quit phrase, space, and arrow that comes after
the client's or server's handle. In this way, the "\quit" message will only be taken as a command
to quit the client connection when entered as "\quit" alone by the user, and not part of a longer
message, like "when you want to quit just type \quit". Will then compare the number of matches of
those characters to the number of chars in the quit phrase. If they're equal the message was just "\quit".
If not, the client or server message was not "\quit". If the message from the client or server was
"\quit", will return true. If not, will return false. Adapted form my own work 11-4-19 for the CS 372
project 1 chatserve program
*/
int QuitCheck(char *message){

	//set bools and counters to false and 0. get length of quit phrase and message
	int isQuit = FALSE;
	int quitCount = 0;
	int k = (strlen(message) - 1);
	int j = (strlen(QUIT_PLUS_DELIMITER) - 1);
	int loopCount = 0;
	int phraseMatchCount = strlen(QUIT_PLUS_DELIMITER);

	//check that string length is long enough to not error
	if(strlen(message) >= phraseMatchCount){
		//loop for length of quit phrase
		while(loopCount < phraseMatchCount){
			//check if chars match, if do, increment count
			if(message[k] == QUIT_PLUS_DELIMITER[j]){
				quitCount++;
			}
			//decrement indexes, increment loop counter
			k--;
			j--;
			loopCount++;
		}
	}
	//check if all chars matched by checking count. if all did, quit was the command, set true.
	if(quitCount == phraseMatchCount){
		isQuit = TRUE;
	}
	//return bool
	return isQuit;
}

/*
pre-conditions: valid command line args from user
post-conditions: 
description: will call all other functions, use addrinfo structs to get socket set up and ready
for connection initiation, create and initialize variables, and do major loop for sending multiple messages
until the client or server enters the "\quit" message, upon which time it will terminate the program.
*/
int main(int argc, char *argv[]){
    ArgCheck(argc, argv);
		
	//using atoi to convert from string to int adapted from:
	//https://www.quora.com/How-do-I-convert-character-value-to-integer-value-in-c-language
	char const *portNum = argv[2];
	char *hostAddress = argv[1];
	//set bools to false
	int goodHandle = FALSE;
	int goodMessage = FALSE;
	//set strings to their max sizes and memset
	char userHandle[MAX_HANDLE_SIZE];
	char sendBuffer[MAX_MSG_PLUS_HANDLE];
	char recvBuffer[MAX_MSG_PLUS_HANDLE];
	memset(userHandle, '\0', sizeof(userHandle));
	memset(sendBuffer, '\0', sizeof(sendBuffer));
	memset(recvBuffer, '\0', sizeof(recvBuffer));

	//get socket ready for initaiting connection. excerpted from:
	//http://beej.us/guide/bgnet/html/#a-simple-stream-client
	int status, socketFD;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	//get handle
	while (goodHandle == FALSE){
		printf("What's your handle?: ");
		fflush(stdin);
		goodHandle = GetHandle(userHandle);
	}

	//get address info using getaddrinfo and check the status. excerpted from:
	//http://beej.us/guide/bgnet/html/#a-simple-stream-client
	status = getaddrinfo(hostAddress, portNum, &hints, &servinfo);
	if (status < 0){
		fprintf(stderr, "Error getting address info.\n"); fflush(stdout); exit(1);
	}	

	//loop as long as user hasn't entered quit
	do{
		//initiate contact w server
		socketFD = InitiateContact(servinfo);
		
		//get message
		while (goodMessage == FALSE){
			printf("%s> ", userHandle);
			fflush(stdin);
			goodMessage = GetMessage(sendBuffer, userHandle);
		}	

		goodMessage = FALSE;
		//send message to server 
		SendMessage(socketFD, sendBuffer);
		//receive message from server
		ReceiveMessage(socketFD, recvBuffer);
		
		//check if server or client have entered quit command otherwise memset
		if ((QuitCheck(sendBuffer) == FALSE) && (QuitCheck(recvBuffer) == FALSE)){
			printf("%s\n", recvBuffer); fflush(stdout);
			memset(sendBuffer, '\0', sizeof(sendBuffer));
			memset(recvBuffer, '\0', sizeof(recvBuffer));
		}
		//check if received quit command, if so disconnect 
		if (QuitCheck(recvBuffer) == TRUE){
			printf("%s\n", recvBuffer); fflush(stdout);
			printf("the client is disconnecting.\n");
		}
		//check if received quit command, if so, disconnect
		if (QuitCheck(sendBuffer) == TRUE){
			printf("the client is disconnecting.\n");
		}
		//close socket. excerpted from: 
		//http://beej.us/guide/bgnet/html/#a-simple-stream-client
		close(socketFD);
	} while((QuitCheck(sendBuffer) == FALSE) && (QuitCheck(recvBuffer) == FALSE));

	//free the server info struct w freeaddrinfo. excerpted from:
	//http://beej.us/guide/bgnet/html/#a-simple-stream-client
	freeaddrinfo(servinfo);
    return 0;
}
