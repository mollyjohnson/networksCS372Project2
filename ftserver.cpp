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
#define LIST "-l"
#define GET "-g"
#define FT_SERVER_HOST_ADDRESS "flip1.engr.oregonstate.edu"
#define FT_CLIENT_HOST_ADDRESS "flip2.engr.oregonstate.edu"

void ArgCheck(int argCount, char *args[]){

	cout << "num args is: " << argCount << " and the arg is: " << args[1] << "\n";

	//arg count must be 2 to be valid
    if (argCount != 2){
        fprintf(stderr, "Wrong number of arguments! Must enter the correct hostname and a valid port number. Start the program again.\n");
        //fflush(stdout); exit(1);    
		exit();
    }
	//using atoi to convert from string to int adapted from:
	//https://www.quora.com/How-do-I-convert-character-value-to-integer-value-in-c-language
	//check if arg is a valid integer (and non negative) and aren't above valid port nums
	/*
	if ((!IntInputValidation(args[2])) || (atoi(args[2]) < 0 ) || (atoi(args[2]) > 65535)){
		fprintf(stderr, "You entered a negative number, string or otherwise invalid port number argument. Start the program again.\n");
       	fflush(stdout); exit(1);    
	}
	*/
}

int main(int argc, char *argv[]){
	ArgCheck(argc, argv);	

	return 0;
}