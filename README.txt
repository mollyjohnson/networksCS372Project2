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
Make sure ftserver.cpp and the ftserver_makefile are present in the directory you're in on flip1.engr.oregonstate.edu,
as well as any text files you want to use for testing with ftserver. And make sure ftclient.py and the ftclient_makefile
are present in the directory you're in on flip2.engr.oregonstate.edu, as well as any text files you want to
use for testing with ftclient. In your flip1.engr.oregonstate.edu window, rename ftserver_makefile into just makefile
(you could type "mv ftserver_makefile makefile". Then type "make". This will compile ftserver.cpp into the
ftserver executable. In your flip2.engr.oregonstate.edu window, rename ftclient_makefile into just makefile (you
could type "mv ftclient_makefile makefile"). Then type "make". This will add +x (executable) permissions to
the ftclient.py file. So in your flip1 window, you should now have an executable called ftserver, and in your flip2
window, you should now have executable permissions added to ftclient.py.

Now, go to your flip1.engr.oregonstate.edu window, and type "./ftserver <controlPortNum>" minus the quotes, where 
<controlPortNum> is some number between 1025 and 65535. This should start up ftserver so it's waiting for
connections on the control port. Now, go back to your flip2.engr.oregonstate.edu window, and type
"./ftclient.py flip1.engr.oregonstate.edu <serverPortNum> <command> <filename> (filename is optional) <dataPortNum>",
where serverPortNum is the same port number you used as the <controlPortNum> when executing ftserver, the command is
-l (if no filename and you want to see ftserver's directory) or -g (if you're going to include a filename and want to
get the contents of that file), the filename is the file in the directory ftserver is in that you want to get the contents
of (if you want a file, you don't have to include a filename if you just want the directory list), and <dataPortNum>
is the port number between 1025 and 65535 you want to use for the data connection.
