/*
Programmer name: Molly Johnson (johnsmol)
Program name: ftserver
Program description: ftserver acts as the server for the control connection portion of
this program, and as the client for the data connection portion of this program. Will
initiate contact w/ ftclient,receive control info from ftclient, validate this information,
then send a response regarding the validity of thiscontrol info to the ftclient and see
if the message received back indicates the control info was acceptable or not and if
it was acceptable, this program will initiate a connection with ftclient, sending either
the contents of the current directory ftserver is in, or the file contents of the requested
filename from ftclient. Will then close the data connection (connection Q) but continue waiting
on host A and the specified control port for client requests until terminated by a user's SIGINT (ctrl-c)
Course name: CS 372 Intro to Networks
Last modified: 12-1-19
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

//constant macro definitions
#define LIST_COMMAND "-l"
#define GET_COMMAND "-g"
//per the assignment instructions, need to make sure ftclient and ftserver are run
//on different flip servers
#define SERVER_HOST_ADDRESS "flip1.engr.oregonstate.edu"
#define CLIENT_HOST_ADDRESS "flip2.engr.oregonstate.edu"
//listed as valid port num range by TA Danielle on slack OSU 372 Fall 2019
#define MAX_PORT 65535
#define MIN_PORT 1025
#define NUM_ARGS 2
//10-20 listed as acceptable backlog on beej's guide, excerpted from:
//http://beej.us/guide/bgnet/html/#a-simple-stream-server
#define BACKLOG 20
//size of 1028 listed on piazza as a good size to use that's big enough to account for
//a line of text but not too big to send with one send() call
#define MAX_MSG_SIZE 1028

/*
pre-conditions: isFile bool prev initialized, command is a valid string, filename is a valid string
post-conditions: will return true if the command was valid (-g before a filename, or -l with
no filename), false if the command was invalid (any other command besides -g before a filename,
or -l with no filename)
description: accepts bool arg previously found indicating whether ftclient included a filename
or not, string of the ftclient command, and a string of the ftclient filename it's requesting.
if isFile bool is true (ftclient gave a filename on the control connection message), will
check if the command is -g. if it is, will return true. otherwise will clear the filename string
since filename would then be invalid since command was invalid, and return false. if the isfile
bool was false (no file being requested by ftclient), will check if the command was -l. if so,
will return true. otherwise will return false. note: -l <filename> and -g with no filename are
NOT considered valid commands.
*/
bool CommandCheck(bool isFile, string command, string &filename){
	//if there was a file, check that command was -g
	if(isFile == true){
		//if command was -g, return true
		if(command == GET_COMMAND){
			return true;
		}	
		//else if command wasn't -g, clear the filename and return false
		//clear the filename string since the command was bad (i.e. wasn't "-g")
		//clearing a string adapted from: http://www.cplusplus.com/reference/string/string/clear/
		filename.clear();
		return false;
	}
	//else if isfile is false, there's just a command from the client and no file
	//check if the command is -l or not
	else{
		//if command was -l, return true. else return false
		if(command == LIST_COMMAND){
			return true;
		}
		return false;
	}
}

/*
pre-conditions: inputstring is a valid string
post-conditions: will return false if any of the chars of the string are not in the range
of ascii values for integers 0-9 (ascii 48-57). if all are within this range, will return true.
description: accepts a string, checks each char in the string to make sure it's a non-negative
integer. returns true if all chars were non-negative ints, otherwise returns false.
*/  
bool IntInputValidation(string inputString){
	//this function was adapted from my own work for OSU CS 344 Winter 2019 assignment 3,
	//last updated 3-3-19.
	
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
pre-conditions: has received some command line args from the user when they executed the program
post-conditions: will exit w code 1 if arg count or the port num args were invalid
description: accepts the argCount and args array as arguments. for ftserver, argcount must be 2,
and arg 1 must be a valid port number. if number of args is wrong, or arg1 is not an integer or
is outside of the valid port number range, will print error message and exit.
*/
void ArgCheck(int argCount, char *args[]){
	//arg count must be 2 to be valid. if not == 2, print error message and exit
    if (argCount != NUM_ARGS){
        fprintf(stderr, "Wrong number of arguments! Must enter a valid port number. Start the program again.\n");
    	fflush(stdout); exit(1);    
    }
	//using atoi to convert from string to int adapted from:
	//https://www.quora.com/How-do-I-convert-character-value-to-integer-value-in-c-language
	//check if arg is a valid integer (and non negative) and aren't above or below valid port nums. if they
	//are, print error message and exit
	if ((!IntInputValidation(args[1])) || (atoi(args[1]) < MIN_PORT) || (atoi(args[1]) > MAX_PORT)){
		fprintf(stderr, "You entered a string or port number outside the valid range (1025-65535). Start the program again.\n");
       	fflush(stdout); exit(1);    
	}
}

/*
pre-conditions: valid socket file descriptor sockFD, valid sockaddr_storage struct.
post-conditions: will return the new socket descriptor upon 
successful accept(), exit if accept() unsuccessful.
description: will accept a socket file descriptor and sockaddr_storage struct. will get
size of the sockaddr_storage and call accept to accept a connection. if error on accept,
will print error message and exit. otherwise will return the new socket descriptor for
the connection.
using sockaddr_storage struct adapted from:  
http://beej.us/guide/bgnet/html/#a-simple-stream-server.
*/
int AcceptConnection(int sockFD, struct sockaddr_storage their_addr){
	//get size of sockaddr_storage struct
	socklen_t addr_size = sizeof their_addr;

	//accept connection. accept() use adapted from:
	//http://beej.us/guide/bgnet/html/#a-simple-stream-server 
	int newFD = accept(sockFD, (struct sockaddr *)&their_addr, &addr_size);

	//if the new socket file descriptor was <0, was an error on accept. print
	//error message and exit. 
	if (newFD < 0){
		fprintf(stderr, "ERROR on accept\n");
		fflush(stdout);
		exit(1);
	}

	//return the new file descriptor
	return newFD;
}

/*
pre-conditions: accepts a port number (as char const not int) and addrinfo struct
post-conditions: returns socket descriptor
description: accepts port num char const and addrinfo struct. uses socket() call
to create socket descriptor. if error creating socket descriptor, prints message
and exits. otherwise will then bind the socket to host address and server port, 
then listen for connections and return the socket descriptor.
*/
int ServerSocketStartup(char const *portNum, struct addrinfo *servinfo){
	//create socket w server info. adapted from:
	//http://beej.us/guide/bgnet/html/#a-simple-stream-server   
	int sockFD = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	//if socket file descriptor <0, was an error
	if (sockFD < 0){
		fprintf(stderr, "Error creating socket descriptor.\n"); fflush(stdout); exit(1);
	}

	//bind the socket to the specified host address and server port. adapted from:
	//http://beej.us/guide/bgnet/html/#a-simple-stream-server 
	bind(sockFD, servinfo->ai_addr, servinfo->ai_addrlen);

	//have server start listening for TCP connection requests from clients. adapted from:
	//http://beej.us/guide/bgnet/html/#a-simple-stream-server  
	listen(sockFD, BACKLOG);

	//return file descriptor
	return sockFD;
}

/*
pre-conditions: valid socket file descriptor, valid string message
post-conditions: will have sent the message using send() unless an error occurred
description: accepts socket descriptor and a string message. send() won't accept a
string so copies string into a char array (but memsets to blank first). will
send up to the length of that char array with send(), and check for send errors.
if send() errors, will print error message and exit.
*/
void SendMessage(int sockFD, string message){
	//using send() with sockets adapted from:
	//http://beej.us/guide/bgnet/html/#a-simple-stream-server and
	//my own work from OSU CS 344 Project 4, Winter 2019
	int charsW = -1;
	//create sending buffer of max size and memset to 0
	char sendBuf[MAX_MSG_SIZE];
	memset(sendBuf, '\0', sizeof(sendBuf));

	//copying a string to an array of chars adapted from:
	//https://www.geeksforgeeks.org/convert-string-char-array-cpp/
	strcpy(sendBuf, message.c_str());

	//cout << "the length of your send buffer is: " << strlen(sendBuf) << "\n";
	charsW = send(sockFD, sendBuf, strlen(sendBuf), 0);

	//check that chars written is >0
	if (charsW < 0){
		fprintf(stderr, "Error writing to socket.\n"); fflush(stdout); exit(1);
	}

	//check that chars written is not < the size of the string
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
pre-conditions: one valid socket descriptor
post-conditions: will return message received from recv() unless recv() errored
description: accepts a socket descriptor arg. creates receive buffer char array,
memsets this array to blank. calls recv() function and checks for recv() error.
if errored, will print message and exit. otherwise will copy char arry into a string,
and return this string.
*/
string ReceiveMessage(int newFD){
	//using send() with sockets adapted from:
	//http://beej.us/guide/bgnet/html/#a-simple-stream-server and
	//my own work from OSU CS 344 Project 4, Winter 2019
	int charsR = -1;
	//create receiving buffer of max size and memset to 0
	char recvBuffer[MAX_MSG_SIZE];
	memset(recvBuffer, '\0', sizeof(recvBuffer));

	//receive message of size max num of message chars plus max num handle chars minus 1
	charsR = recv(newFD, recvBuffer, (MAX_MSG_SIZE - 1), 0);
	
	//check if the num of chars received is <0
	if (charsR < 0){
		fprintf(stderr, "Error reading from the socket.\n"); fflush(stdout); exit(1);
	}

	//set the message equal to the receive buffer char array and return the string
	string message = recvBuffer;
	return message;
}

/*
pre-conditions: needs a valid string, valid char (that's not printable since i can't know what chars will
be in the text files the graders will be using), a valid string command, a valid string filename, and a valid string
port variable. 
post-conditions: command, filename, and dataPort variables passed by reference, their values will be updated
by end of this function unless is an error. returns true if filename present, false if no filename.
description: accepts the large string msg received from ftclient (separated by delimiters). creates a vector
and stringstream. uses getline and the special delimiter to tokenize the substrings from the larger control
connection message string recv'd from ftclient. pushes each of these intermediate substrings onto the vect.
if are 2 vector strings, first is token, second is dataport, filename not present so clears filename variable
and returns false. if are 3 vector strings, first is command, second is filename, third is dataport and returns
true since a filename was present.
*/
bool ParseControlMessage(string controlMsgRecd, char delimiter, string &command, string &filename, string &dataPort){
	//using stringstream and getline to get a vector of parsed strings separated by a delimiter character
	//is adapted from: https://www.geeksforgeeks.org/tokenizing-a-string-cpp/
	vector <string> tokens;
	stringstream check1(controlMsgRecd);
	string intermediate;

	//while getline checks the stringstream for the specified delimiter, take each intermediate string (each string chunk
	//between delimiters) and push it back onto a vector
	while(getline(check1, intermediate, delimiter)){
		tokens.push_back(intermediate);
	}

	//check the tokens vector (vector of substrings that were separated by the delimiter). the size should be either 2 or 3.
	//if the size is 2, set command var to first vector index, dataport var to second vect index, and clear filename as 
	//there was no filename. return false (since there was no filename)
	if (tokens.size() == 2){
		command = tokens[0];
		dataPort = tokens[1];
		filename.clear();
		return false;
	}
	//else if the size is 3, set command var to first vector index, filename to second vect index, and dataport to third
	//vect index. return true since there was a filename
	else if (tokens.size() == 3){
		command = tokens[0];
		filename = tokens[1];
		dataPort = tokens[2];
		return true;
	}
	//else if the vect size wasn't 2 or 3, was an error, print error msg and exit
	else{
		fprintf(stderr,"something went wrong here, control message tokens vect size \n");
		fflush(stdout); exit(1);
	}
}

/*
pre-conditions: valid addrinfo struct
post-conditions: returns a socket descriptor
description: function accepts an addrinfo struct. creates socket w socket() call.
checks for error, if error will print msg and exit. connects using connect(), checks
status of connect call, if error will print msg and exit. if both successful, returns
socket descriptor.
*/
int InitiateContact(struct addrinfo *servinfoData){
	//using send() with sockets adapted from:
	//http://beej.us/guide/bgnet/html/#a-simple-stream-client
	
	//create socket descriptor
	int sockFD = socket(servinfoData->ai_family, servinfoData->ai_socktype, servinfoData->ai_protocol);

	//check socket descriptor creation was ok. if <0, print error msg and exit
	if(sockFD < 0){
		fprintf(stderr, "Error creating socket descriptor.\n"); fflush(stdout); exit(1);
	}

	//connect w socket and check status. if status <0, print error msg and exit
	int statusConnect = connect(sockFD, servinfoData->ai_addr, servinfoData->ai_addrlen);
	if(statusConnect < 0){
		fprintf(stderr, "Error connecting to server.\n"); fflush(stdout); exit(1);
	}

	//return socket descriptor
	return sockFD;
}

/*
pre-conditions: valid vector of strings
post-conditions: vector passed by reference, vect values will be updated by this function
description: 
*/
void GetDirectoryContents(vector<string> &directoryContents){
	//create fixed size array for current working directory and memset
	char cwd[MAX_MSG_SIZE];
	memset(cwd, '\0', sizeof(cwd));

	//getting contents of current working directory adapted from:
	//https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
	//if getcwd returns null, couldn't access current working directory, print error msg and exit.
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
		//(NULL means all contents of directory have been read and none left to read)
		while((directory = readdir(dir)) != NULL){
			//for each item of content in the directory, add that file name to the 
			//directory contents vector
			directoryContents.push_back(directory->d_name);
		}
		//close the directory
		closedir(dir);
	}
	//else if the directory couldn't be open, print error message and exit
	else{
		fprintf(stderr, "Error, could not open directry\n"); fflush(stdout); exit(1);
	}
}

/*
pre-conditions:
post-conditions:
description:
*/
void GetFileContents(vector<string> &fileContents, string filename, int socketFDData, char delimiter){
	//getting file contents line by line adapted from:
	//https://stackoverflow.com/questions/7868936/read-file-line-by-line-using-ifstream-in-c and
	//my own work from CS 325 Algorithms at OSU, last updated 8-16-18

	//create ifstream var, a string for each line, and open the file
	ifstream inputFile;
	string line;
	inputFile.open(filename.c_str());

	//check that file opened correctly. if not print error msg and exit
	if(!inputFile){
		fprintf(stderr, "File not opened correctly.\n");
		fflush(stdout); exit(1);
	}

	//for each line in the file, push it back onto the vector
	while(getline(inputFile, line)){
		fileContents.push_back(line);
	}
	
	//close the file
	inputFile.close();
}

/*
pre-conditions:
post-conditions:
description:
*/
int main(int argc, char *argv[]){
	//check num args and arg validity
	ArgCheck(argc, argv);	

	//after arg check, set control port number variable
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

	//varios variables for the functions declared/initialized
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

	//control connection get addr info
	statusControl = getaddrinfo(SERVER_HOST_ADDRESS, controlPort, &hintsControl, &servinfoControl);
	if (statusControl < 0){
		fprintf(stderr, "Error getting address info.\n"); fflush(stdout); exit(1);
	}

	//startup server for control connection
	socketFDControl = ServerSocketStartup(controlPort, servinfoControl);
	cout << "Server open on " << controlPort << "\n";

	//loop until SIGINT (ctrl-c) is received
	while(1){
		//accept control connection
		newSocketFDControl = AcceptConnection(socketFDControl, their_addr);
		cout << "Connection from " << CLIENT_HOST_ADDRESS << ".\n";

		//receive message from ftclient over control connection
		controlMsgRecd = ReceiveMessage(newSocketFDControl);

		//check if there's a filename sent by the client or not and parse out the message
		//using the delimiter so can assign those variables
		isFile = ParseControlMessage(controlMsgRecd, delimiter, command, filename, dataPortString);

		//initializeconst char dataPort 
		char const *dataPort = dataPortString.c_str();

		//check if the command was valid (either "-l" or "-g <FILENAME>")
		goodCommand = CommandCheck(isFile, command, filename);

		//if client sent an invalid command, i.e. not "-l" or "-g <FILENAME>", send error message to ftclient
		//over control connection
		if(goodCommand == false){
			string errorMessage = "Error, that command was invalid. Please use \"-l\" or \"-g <FILENAME>\"\n";
			SendMessage(newSocketFDControl, errorMessage);
		}

		//else if there was a file and the command was -g
		else if((isFile == true) && (command == GET_COMMAND)){
			cout << "File \"" << filename << "\" requested on port " << dataPort << ".\n";

			//check if the filename requested is present in the same directory as ftserver
			GetDirectoryContents(directoryContents);

			int foundFileCount = 0;
			//check if any of the directory contents match file name. if so, increment foundCount
			for(int k = 0; k < directoryContents.size(); k++){
				if(directoryContents[k] == filename){
					foundFileCount++;
				}	
			}	
			//if no filename count increments (matches) were found, file not found, send error msg
			//to ftclient over control connection and set filefound to false
			if(foundFileCount == 0){
				string errorMessage = "File not found.\n";
				cout << "File not found. Sending error message to " << CLIENT_HOST_ADDRESS 
						<< ":" << dataPort << "\n";

				SendMessage(newSocketFDControl, errorMessage);
				fileFound = false;
			}
			//otherwise if filename count increments >0 (file was foud), set filefound to true
			else{
				fileFound = true;
			}

			//erasing a vector so it's empty again excerpted from:
			//https://www.geeksforgeeks.org/vector-erase-and-clear-in-cpp/ and
			//http://www.cplusplus.com/reference/vector/vector/erase/
			//erasing a vector so it's empty again excerpted from:
			//https://www.geeksforgeeks.org/vector-erase-and-clear-in-cpp/ and
			//http://www.cplusplus.com/reference/vector/vector/erase/
			//erase vect contents in case loops around again w a new connection
			directoryContents.erase(directoryContents.begin(), directoryContents.end());

			//if a file was found
			if(fileFound == true){

				//set up TCP data connection with ftclient (ftclient is server in this case so use their host address,
				//and the port should be the data port not the control port)
				//adapted from: http://beej.us/guide/bgnet/html/#a-simple-stream-client
				statusData = getaddrinfo(CLIENT_HOST_ADDRESS, dataPort, &hintsData, &servinfoData);	
				
				//check status, if getaddrinfo returned <0, print error and exit
				if(statusData < 0){
					fprintf(stderr, "Error getting address info.\n"); fflush(stdout); exit(1);
				}

				//initiate contact w ftclient (ftclient now acting as a server) over the data connection
				socketFDData = InitiateContact(servinfoData);
				
				//get the contents of the requested file
				GetFileContents(fileContents, filename, socketFDData, delimiter);
				cout << "Sending \"" << filename << "\" to" << CLIENT_HOST_ADDRESS <<
						":" << dataPort << "\n";

				for(int k = 0; k < fileContents.size(); k++){
					//if not the last item in the vector, add newline char and send to ftclient
					if(k != fileContents.size() - 1){
						SendMessage(socketFDData, (fileContents[k] + "\n"));
					}
					//else if the last of the messages being sent, append special delimiter char instead of a newline
					else{
						SendMessage(socketFDData, (fileContents[k] + delimiter));
					}
				}
				//erase file contents vect in case loops around again w a new client connection later
				fileContents.erase(fileContents.begin(), fileContents.end());

				//close the data socket (connection Q)
				close(socketFDData);
			}
		}
		//else the command was "-l"
		else{
			//set up TCP data connection with ftclient (ftclient is server in this case so use their host address,
			//and the port should be the data port not the control port)
			//adapted from: http://beej.us/guide/bgnet/html/#a-simple-stream-client
			cout << "List directory requested on port " << dataPort << ".\n";
			statusData = getaddrinfo(CLIENT_HOST_ADDRESS, dataPort, &hintsData, &servinfoData);	

			//check status, if getaddrinfo returned <0, print error and exit
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
			cout << "Sending directory contents to " << CLIENT_HOST_ADDRESS << ":" 
					<< dataPort << "\n";
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
			directoryContents.erase(directoryContents.begin(), directoryContents.end());

			//close the data socket
			close(socketFDData);
		}
		//close new socket for control	
		close(newSocketFDControl);
	}
	//freeaddrinfo(servinfoData);
	freeaddrinfo(servinfoControl);

	return 0;
}
