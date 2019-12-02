#Programmer Name: Molly Johnson (johnsmol)
#Program Name: makefile 
#Program Description: makefile that will compile the .cpp file into an executable and
#add executable permissions to the python file
#Course Name: CS 372 Intro to Networks
#Last Modified: 12-1-19

#makefile adapted from  my own work during OSU CS 344 Winter 2019, last updated 3/3/19.
#make all be the default target, with the dependency of FTP 
all: FTP 

#FTP target with the dependency of ftserver.cpp and command to compile ftserver.cpp into
#the ftserver executable using g++. Also includes the chmod +x command to add executable permissions
#to the ftclient file (which is a python file) so that this script can be executed without needing to specify
#the python version when trying to run the script (i.e. you could do ./ftclient insteady of python3 ./ftclient)
FTP: ftserver.cpp
	g++ -std=c++11 ftserver.cpp -o ftserver
	chmod +x ftclient.py
	
#if the user enters "make clean" or "make cleanall", remove the ftserver executable and
#remove the executable permissions from the ftclient python file.
clean:
	rm -f ftserver
	chmod -x ftclient.py
cleanall: 
	rm -f ftserver
	chmod -x ftclient.py
