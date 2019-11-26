/*
Programmer name: Molly Johnson (johnsmol)
Program name: ftserver
Program description:
Course name: CS 372 Intro to Networks
Last modified:
*/

//include libraries
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <cmath>
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
#include <bits/stdc++.h>

//specify things used from std instead of using standard namespace
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::getline;
using std::stringstream;
using std::istringstream;
using std::basic_string;

//macro definitions
#define LIST_COMMAND "-l"
#define GET_COMMAND "-g"
#define SERVER_HOST_ADDRESS "flip1.engr.oregonstate.edu"
#define CLIENT_HOST_ADDRESS "flip2.engr.oregonstate.edu"
#define MAX_PORT 65535
#define MIN_PORT 1025
#define NUM_ARGS 2
#define BACKLOG 20
#define RECV_BUF_SIZE 1000

/*
pre-conditions:
post-conditions:
description:
*/
bool CommandCheck(bool isFile, string command){
	if(isFile == true){
		if(command == GET_COMMAND){
			return true;
		}	
		return false;
	}
	//else if isfile is false, there's just a command from the client and no file
	//check if the command is -l or not
	else{
		if(command == LIST_COMMAND){
			return true;
		}
		return false;
	}
}

/*
pre-conditions:
post-conditions:
description:
*/  
int IntInputValidation(string inputString){
	//create variables for char to be checked, loop counter, and length of the input string
	char asciiValue;
	int inputLength = inputString.length();

	//loop through entire string so can check each char
	for(int i = 0; i < inputLength; i++){
		//set temp char equal to the current string char
		asciiValue = inputString[i];

		//check if the current char is within the valid ascii values for ints 0-9 (i.e. ascii values 48-57)
		if((asciiValue < 48) || (asciiValue > 57)){
			//if ascii value of char being checked is outside of the range for integers, return false
			return false;
		}
	}
	//if all of the chars in the string to be checked were valid integers, return true
	return true;
}

/*
pre-conditions:
post-conditions:
description:
*/
void ArgCheck(int argCount, char *args[]){
	//arg count must be 2 to be valid
    if (argCount != NUM_ARGS){
        fprintf(stderr, "Wrong number of arguments! Must enter a valid port number. Start the program again.\n");
    	fflush(stdout); exit(1);    
    }
	//using atoi to convert from string to int adapted from:
	//https://www.quora.com/How-do-I-convert-character-value-to-integer-value-in-c-language
	//check if arg is a valid integer (and non negative) and aren't above valid port nums
	if ((!IntInputValidation(args[1])) || (atoi(args[1]) < MIN_PORT) || (atoi(args[1]) > MAX_PORT)){
		fprintf(stderr, "You entered a string or port number outside the valid range (1025-65535). Start the program again.\n");
       	fflush(stdout); exit(1);    
	}
}

/*
pre-conditions:
post-conditions:
description:
*/
int AcceptConnection(int sockFD, struct sockaddr_storage their_addr){
	socklen_t addr_size = sizeof their_addr;
	int newFD = accept(sockFD, (struct sockaddr *)&their_addr, &addr_size);
	if (newFD < 0){
		fprintf(stderr, "ERROR on accept\n");
		fflush(stdout);
		exit(1);
	}
	return newFD;
}

/*
pre-conditions:
post-conditions:
description:
*/
int ServerSocketStartup(char const *portNum, struct addrinfo *servinfo){
	//create socket w server info
	int sockFD = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	//if socket file descriptor <0, was an error
	if (sockFD < 0){
		fprintf(stderr, "Error creating socket descriptor.\n"); fflush(stdout); exit(1);
	}
	//bind the socket to the specified host address and server port
	bind(sockFD, servinfo->ai_addr, servinfo->ai_addrlen);
	//have server start listening for TCP connection requests from clients
	listen(sockFD, BACKLOG);

	return sockFD;
}

/*
pre-conditions:
post-conditions:
description:
*/
void SendMessage(int sockFD, string message){
	int charsW = -1;
	//send message to server
	char sendBuf[RECV_BUF_SIZE];
	memset(sendBuf, '\0', sizeof(sendBuf));
	//copying a string to an array of chars adapted from:
	//https://www.geeksforgeeks.org/convert-string-char-array-cpp/
	strcpy(sendBuf, message.c_str());
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
pre-conditions:
post-conditions:
description:
*/
string ReceiveMessage(int newFD){
	int charsR = -1;
	char recvBuffer[RECV_BUF_SIZE];
	memset(recvBuffer, '\0', sizeof(recvBuffer));

	//receive message of size max num of message chars plus max num handle chars minus 1
	charsR = recv(newFD, recvBuffer, (RECV_BUF_SIZE - 1), 0);
	//check if the num of chars received is <0
	if (charsR < 0){
		fprintf(stderr, "Error reading from the socket.\n"); fflush(stdout); exit(1);
	}

	string message = recvBuffer;
	return message;
}

/*
pre-conditions:
post-conditions:
description:
*/
bool ParseControlMessage(string controlMsgRecd, char delimiter, string &command, string &filename){
	//using stringstream and getline to get a vector of parsed strings separated by a delimiter character
	//is adapted from: https://www.geeksforgeeks.org/tokenizing-a-string-cpp/
	vector <string> tokens;
	stringstream check1(controlMsgRecd);
	string intermediate;

	while(getline(check1, intermediate, delimiter)){
		tokens.push_back(intermediate);
	}
	cout << "the size of your tokens vector is: " << tokens.size() << "\n";
	if (tokens.size() == 1){
		command = tokens[0];
		//filename = "ERROR";
		return false;
	}
	if (tokens.size() > 1){
		for (int i = 0; i < tokens.size(); i++){
			if(i == 0){
				command = tokens[i];
			}	
			else if(i == 1){
				filename = tokens[i];
			}
			else{
				fprintf(stderr,"something went wrong, your control message vector has more than 2 split messages\n");
				fflush(stdout); exit(1);
			}
		}
		return true;
	}
	else{
		fprintf(stderr,"something went wrong here, control message tokens vect size < 1\n");
		fflush(stdout); exit(1);
	}
}

/*
pre-conditions:
post-conditions:
description:
*/
int main(int argc, char *argv[]){
	ArgCheck(argc, argv);	
	char const *controlPort = argv[1];

	int statusControl, socketFDControl, newSocketFDControl;
	struct addrinfo hintsControl;
	struct addrinfo *servinfoControl;
	memset(&hintsControl, 0, sizeof(hintsControl));
	hintsControl.ai_family = AF_UNSPEC;
	hintsControl.ai_socktype = SOCK_STREAM;
	hintsControl.ai_flags = AI_PASSIVE;
	struct sockaddr_storage their_addr;

	string controlMsgRecd;
	string command;
	string filename;
	bool isFile = false;
	bool goodCommand = false;

	//using a non-printable ascii control character as a delimiter to separate messages
	//so that there's no chance of the delimiter being present in the command name, file
	//name, or file contents (ascii 3 is "end of text" control character) 
	//list of ascii control characters found from:
	//https://www.ascii-code.com/
	char delimiter = 3;

	//control connection socket startup
	statusControl = getaddrinfo(SERVER_HOST_ADDRESS, controlPort, &hintsControl, &servinfoControl);
	if (statusControl < 0){
		fprintf(stderr, "Error getting address info.\n"); fflush(stdout); exit(1);
	}

	socketFDControl = ServerSocketStartup(controlPort, servinfoControl);

	//loop until SIGINT (ctrl-c) is received
	while(1){
		newSocketFDControl = AcceptConnection(socketFDControl, their_addr);
		controlMsgRecd = ReceiveMessage(newSocketFDControl);
		cout << "the received control message from the client is: " << controlMsgRecd << "\n";

		//check if there's a filename sent by the client or not
		isFile = ParseControlMessage(controlMsgRecd, delimiter, command, filename);

		//if isfile is true, means there's a command and a file sent from the client.
		//check if the command is -g or not
		goodCommand = CommandCheck(isFile, command);

		//if client sent an invalid command, i.e. not "-l" or "-g <FILENAME>"
		if(goodCommand == false){
			string errorMessage = "Error, that command was invalid. Please use \"-l\" or \"-g <FILENAME>\"\n";
			SendMessage(newSocketFDControl, errorMessage);
		}
		
		cout << "the command out of loop is: " << command << "\n";
		cout << "the filename out of loop is: " << filename << "\n";
		cout << "the result of isFile bool is: " << isFile << "\n";
		cout << "the result of goodCommand bool is: " << goodCommand << "\n";
		close(newSocketFDControl);
	}
	freeaddrinfo(servinfoControl);

	return 0;
}
