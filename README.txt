Programmer Name: Molly Johnson (johnsmol)
Program Name: README.txt
Program Description: README.txt that will describe to the user how to compile, execute,
and control the programs. 
Course Name: CS 372 Intro to Networks
Last Modified: 12-1-19

Instructions for how to compile, execute, and control the program:
Per assignment instructions, ftserver and ftclient must be run on different OSU flip servers.
Open flip1.engr.oregonstate.edu for ftserver in one window using something like putty to ssh into flip1.
Open flip2.engr.oregonstate.edu for ftclient in another window using something like putty to ssh into flip2.
Make sure ftserver.cpp and the makefile are present in the directory you're in on flip1.engr.oregonstate.edu,
as well as any text files you want to use for testing with ftserver. And make sure ftclient.py and the makefile
are present in the directory you're in on flip2.engr.oregonstate.edu, as well as any text files you want to
use for testing with ftclient. In either the flip1.engr.oregonstate.edu window or the flip2.engr.oregonstate.edu
window (either one, it doesn't matter), type "make" minus the quotes. This will compile ftserver.cpp into the
ftserver executable, and give -x permissions (executable permissions) via chmod +x to the ftclient.py file.
Now, go to your flip1.engr.oregonstate.edu window, and type "./ftserver <controlPortNum>" minus the quotes, where 
<controlPortNum> is some number between 1025 and 65535. This should start up ftserver so it's waiting for
connections on the control port. Now, go back to your flip2.engr.oregonstate.edu window, and type 
