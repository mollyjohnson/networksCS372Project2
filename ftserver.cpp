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

//macro definitions
#define LIST_COMMAND "-l"
#define GET_COMMAND "-g"
#define SERVER_HOST_ADDRESS "flip1.engr.oregonstate.edu"
#define CLIENT_HOST_ADDRESS "flip2.engr.oregonstate.edu"
#define MAX_PORT 65535
#define MIN_PORT 1025
#define NUM_ARGS 2

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
int SocketStartup(int portNum){
	/*#socket server setup excerpted from OSU CS 372 lecture 15 slides (specifically, slide 9)
	#and p.205 from Computer Networking-A Top-Down Approach by Kurose and Ross, 7th ed
	#create socket for server (SOCK_STREAM indicates this is a TCP connection)
	serverSocket = socket(AF_INET, SOCK_STREAM)

	#bind the socket to the specified host address and server port
	serverSocket.bind((hostAddress, serverPort))

	#have server start listening for TCP connection requests from clients
	serverSocket.listen(1)

	return serverSocket
	*/
}

/*
pre-conditions:
post-conditions:
description:
*/
void SendMessage(int socketFD, string message){

}

/*
pre-conditions:
post-conditions:
description:
*/
void ReceiveMessage(int socketFD, string message){

}

/*
pre-conditions:
post-conditions:
description:
*/
int main(int argc, char *argv[]){
	ArgCheck(argc, argv);	
	char const *servPortNum = argv[1];

	int statusControl, socketFDControl;
	struct addrinfo hintsControl;
	struct addrinfo *servinfoControl;
	memset(&hintsControl, 0, sizeof(hintsControl));
	hintsControl.ai_family = AF_UNSPEC;
	hintsControl.ai_socktype = SOCK_STREAM;
	hintsControl.ai_flags = AI_PASSIVE;

	//control connection socket startup
	statusControl = getaddrinfo(SERVER_HOST_ADDRESS, servPortNum, &hintsControl, &servinfoControl);
	if (statusControl < 0){
		fprintf(stderr, "Error getting address info.\n"); fflush(stdout); exit(1);
	}

	socketFDControl = SocketStartup(atoi(argv[1]));

	while(1){

	}

	return 0;
}