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
#define MAX_MSG_SIZE 1028

/*
pre-conditions:
post-conditions:
description:
*/
bool CommandCheck(bool isFile, string command, string &filename){
	if(isFile == true){
		if(command == GET_COMMAND){
			return true;
		}	
		//clear the filename string since the command was bad (i.e. wasn't "-g")
		//clearing a string adapted from: http://www.cplusplus.com/reference/string/string/clear/
		filename.clear();
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
	cout << "YOU'RE IN SEND_MESSAGE\n";
	int charsW = -1;
	//send message to serjver
	char sendBuf[MAX_MSG_SIZE];
	memset(sendBuf, '\0', sizeof(sendBuf));
	//copying a string to an array of chars adapted from:
	//https://www.geeksforgeeks.org/convert-string-char-array-cpp/
	strcpy(sendBuf, message.c_str());
	cout << "the length of your send buffer is: " << strlen(sendBuf) << "\n";
	//cout << "the sendBuf prior to writing to socket is: " << sendBuf << "\n";
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
	char recvBuffer[MAX_MSG_SIZE];
	memset(recvBuffer, '\0', sizeof(recvBuffer));

	//receive message of size max num of message chars plus max num handle chars minus 1
	charsR = recv(newFD, recvBuffer, (MAX_MSG_SIZE - 1), 0);
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
bool ParseControlMessage(string controlMsgRecd, char delimiter, string &command, string &filename, string &dataPort){
	//using stringstream and getline to get a vector of parsed strings separated by a delimiter character
	//is adapted from: https://www.geeksforgeeks.org/tokenizing-a-string-cpp/
	vector <string> tokens;
	stringstream check1(controlMsgRecd);
	string intermediate;

	while(getline(check1, intermediate, delimiter)){
		tokens.push_back(intermediate);
	}
	cout << "the size of your tokens vector is: " << tokens.size() << "\n";
	if (tokens.size() == 2){
		command = tokens[0];
		dataPort = tokens[1];
		filename.clear();
		return false;
	}
	else if (tokens.size() == 3){
		command = tokens[0];
		filename = tokens[1];
		dataPort = tokens[2];
		return true;
	}
	else{
		fprintf(stderr,"something went wrong here, control message tokens vect size \n");
		fflush(stdout); exit(1);
	}
}

/*
pre-conditions:
post-conditions:
description:
*/
int InitiateContact(struct addrinfo *servinfoData){
	int sockFD = socket(servinfoData->ai_family, servinfoData->ai_socktype, servinfoData->ai_protocol);
	if(sockFD < 0){
		fprintf(stderr, "Error creating socket descriptor.\n"); fflush(stdout); exit(1);
	}
	int statusConnect = connect(sockFD, servinfoData->ai_addr, servinfoData->ai_addrlen);
	if(statusConnect < 0){
		fprintf(stderr, "Error connecting to server.\n"); fflush(stdout); exit(1);
	}
	return sockFD;
}

/*
pre-conditions:
post-conditions:
description:
*/
/*
void SendMessageData(int socketFDData, string dataMessage){
	int charsW = -1;
	//send message to serjver
	char sendBuf[MAX_MSG_SIZE];
	memset(sendBuf, '\0', sizeof(sendBuf));
	//copying a string to an array of chars adapted from:
	//https://www.geeksforgeeks.org/convert-string-char-array-cpp/
	strcpy(sendBuf, dataMessage.c_str());
	cout << "the sendBuf prior to writing is: " << sendBuf << "\n";
	//charsW = send(socketFDData, sendBuf, strlen(sendBuf), 0);
	charsW = send(socketFDData, sendBuf, MAX_MSG_SIZE, 0);
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
		ioctl(socketFDData, TIOCOUTQ, &checkSend);
	} while(checkSend > 0);

	//if checksend is <0, was an error
	if (checkSend < 0){
		fprintf(stderr, "Ioctl error.\n"); fflush(stdout); exit(1);
	}
}
*/

/*
pre-conditions:
post-conditions:
description:
*/
void GetDirectoryContents(vector<string> &directoryContents){
	char cwd[MAX_MSG_SIZE];
	memset(cwd, '\0', sizeof(cwd));

	//getting contents of current working directory adapted from:
	//https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
	if(getcwd(cwd, sizeof(cwd)) == NULL){
		fprintf(stderr, "couldn't get current working directory.\n"); fflush(stdout); exit(1);
	}

	//using DIR and struct dirent to open and get the file names in a directory adapted from:
	//http://www.martinbroadhurst.com/list-the-files-in-a-directory-in-c.html
	//create DIR file descriptor
	DIR *dir;
	//create directory struct
	struct dirent *directory;
	//open the current working directory
	dir = opendir(cwd);
	//if the directory opened successfully (dir would == NULL if unable to open)
	if(dir != NULL){
		//read contents of the directory for as long as the contents don't == NULL
		while((directory = readdir(dir)) != NULL){
			//for each item of content in the directory, add that file name to the 
			//directory contents vector
			directoryContents.push_back(directory->d_name);
		}
		//close the directory
		closedir(dir);
	}
	//else if the directory couldn't be open, print error message
	else{
		fprintf(stderr, "Error, could not open directry\n"); fflush(stdout); exit(1);
	}
}

/*
pre-conditions:
post-conditions:
description:
*/
void GetFileContents(vector<string> &fileContents, string filename){
	//getting file contents line by line adapted from:
	//https://stackoverflow.com/questions/7868936/read-file-line-by-line-using-ifstream-in-c and
	//my own work from CS 325 Algorithms at OSU, last updated 8-16-18

	ifstream inputFile;
	string line;
	inputFile.open(filename.c_str());
	if(!inputFile){
		fprintf(stderr, "File not opened correctly.\n");
		fflush(stdout); exit(1);
	}
	while(getline(inputFile, line)){
		//printf("%s\n", line.c_str());
		fileContents.push_back(line);
	}
	inputFile.close();
}

/*
pre-conditions:
post-conditions:
description:
*/
int main(int argc, char *argv[]){
	ArgCheck(argc, argv);	
	char const *controlPort = argv[1];

	//control socket setup info
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
	string dataPortString;
	vector<string> directoryContents;
	bool fileFound = false;
	vector<string> fileContents;

	//data socket setup info
	int statusData, socketFDData, newSocketFDData;
	struct addrinfo hintsData;
	struct addrinfo *servinfoData;
	memset(&hintsData, 0, sizeof(hintsData));
	hintsData.ai_family = AF_UNSPEC;
	hintsData.ai_socktype = SOCK_STREAM;
	hintsData.ai_flags = AI_PASSIVE;

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
		isFile = ParseControlMessage(controlMsgRecd, delimiter, command, filename, dataPortString);

		//const char dataPort here
		char const *dataPort = dataPortString.c_str();
		cout << "the data port out of loop is: " << dataPortString << "\n";
		cout << "the data port out of loop (const char) is: " << dataPort << "\n";

		//check if the command was valid (either "-l" or "-g <FILENAME>")
		goodCommand = CommandCheck(isFile, command, filename);

		//if client sent an invalid command, i.e. not "-l" or "-g <FILENAME>"
		if(goodCommand == false){
			string errorMessage = "Error, that command was invalid. Please use \"-l\" or \"-g <FILENAME>\"\n";
			SendMessage(newSocketFDControl, errorMessage);
		}
		//else if there was a file and the command was -g
		else if((isFile == true) && (command == GET_COMMAND)){
			//check if the filename requested is present in the same directory as ftserver.cpp
			GetDirectoryContents(directoryContents);
			int foundFileCount = 0;
			for(int k = 0; k < directoryContents.size(); k++){
				if(directoryContents[k] == filename){
					foundFileCount++;
					cout << "file found count is: " << foundFileCount << "\n";
				}	
			}	
			cout << "final file found count out of the loop is: " << foundFileCount << "\n";
			if(foundFileCount == 0){
				string errorMessage = "File not found.\n";
				SendMessage(newSocketFDControl, errorMessage);
				fileFound = false;
			}
			else{
				fileFound = true;
			}
			//erasing a vector so it's empty again excerpted from:
			//https://www.geeksforgeeks.org/vector-erase-and-clear-in-cpp/ and
			//http://www.cplusplus.com/reference/vector/vector/erase/
			directoryContents.erase(directoryContents.begin(), directoryContents.end());

			if(fileFound == true){
				//set up TCP data connection with ftclient (ftclient is server in this case so use their host address,
				//and the port should be the data port not the control port)
				statusData = getaddrinfo(CLIENT_HOST_ADDRESS, dataPort, &hintsData, &servinfoData);	
				if(statusData < 0){
					fprintf(stderr, "Error getting address info.\n"); fflush(stdout); exit(1);
				}
				//initiate contact w ftclient (ftclient now acting as a server) over the data connection
				socketFDData = InitiateContact(servinfoData);
				GetFileContents(fileContents, filename);
				cout << "file contents vector size is: " << fileContents.size() << "\n";
				for(int k = 0; k < fileContents.size(); k++){
					//cout << (fileContents[k] + "\n");
					//if not the last item in the vector, add newline char and send to ftclient
					if(k != fileContents.size() - 1){
						SendMessage(socketFDData, (fileContents[k] + "\n"));
					}
					//if the last one of the messages being sent, append special delimiter char instead of a newline
					else{
						SendMessage(socketFDData, (fileContents[k] + delimiter));
					}
				}
				fileContents.erase(fileContents.begin(), fileContents.end());
				close(socketFDData);
			}
		}
		//else the command was "-l"
		else{
			//set up TCP data connection with ftclient (ftclient is server in this case so use their host address,
			//and the port should be the data port not the control port)
			statusData = getaddrinfo(CLIENT_HOST_ADDRESS, dataPort, &hintsData, &servinfoData);	
			if(statusData < 0){
				fprintf(stderr, "Error getting address info.\n"); fflush(stdout); exit(1);
			}
			//initiate contact w ftclient (ftclient now acting as a server) over the data connection
			socketFDData = InitiateContact(servinfoData);

			//if the command ws "-l"
			//retrieve directory contents and put them into the directory contents vector
			GetDirectoryContents(directoryContents);
			
			//send each item in the directory contents vector, adding a newline char to each item
			//unless it's the last item in the directory contents vector, in which case add the
			//special delimiter char so ftclient knows the end of the data sent has been reached
			for(int k = 0; k < directoryContents.size(); k++){
				//if not the last item in the vector, add newline char and send to ftclient
				if(k != directoryContents.size() - 1){
					SendMessage(socketFDData, (directoryContents[k] + "\n"));
				}
				//if the last one of the messages being sent, append special delimiter char instead of a newline
				else{
					SendMessage(socketFDData, (directoryContents[k] + delimiter));
				}
			}
			//erasing a vector so it's empty again excerpted from:
			//https://www.geeksforgeeks.org/vector-erase-and-clear-in-cpp/ and
			//http://www.cplusplus.com/reference/vector/vector/erase/
			directoryContents.erase(directoryContents.begin(), directoryContents.end());
			//close the data socket
			close(socketFDData);
		}
		/*
		cout << "the command out of loop is: " << command << "\n";
		cout << "the filename out of loop is: " << filename << "\n";
		cout << "the result of isFile bool is: " << isFile << "\n";
		cout << "the result of goodCommand bool is: " << goodCommand << "\n";
		*/
		close(newSocketFDControl);
	}
	//freeaddrinfo(servinfoData);
	freeaddrinfo(servinfoControl);

	return 0;
}
