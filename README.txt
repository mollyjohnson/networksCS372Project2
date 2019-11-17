Programmer Name: Molly Johnson (johnsmol)
Program Name: README.txt
Program Description: README.txt that will describe to the user how to compile, execute,
and control the programs. 
Course Name: CS 372 Intro to Networks
Last Modified: 11/04/19

Instructions for how to compile, execute, and control the program:
Log onto the Oregon State flip1 server terminal (flip1.engr.oregonstate.edu) using an SSH client like Putty.
In a separate window, log onto the Oregon State flip2 or flip3 terminal (either one, just so long as it's different
from the flip1 server as the chatserver will be on flip1 and thus the chatclient must be on a different flip, so either
flip2 or flip3). Use flip2.engr.oregonstate.edu for flip2 or flip3.engr.oregonstate.edu for flip3. Copy all of my program's
files into some folder on the Oregon State server (README.txt, makefile, chatclient.c, and chatserve). Note: the chatserve
file is a python file, even though it doesn't have the .py file extension, the shebang at the top of the chatserve file
will make sure the script is run with python3. Now that all 4 files are in some folder on the OSU server (AND you're in that directory),
type "make" (minus the quotes), and this will compile the chatclient.c file into an executable file just called chatclient. The
makefile will also instruct the python file (chatserve) to have its permissions modified so the file can be executable (through a 
chmod +x command in the makefile ). You should now see two files, chatclient and chatserve, each with execution permissions. Now, you
are ready to start the chat application. In the window for flip1, type "./chatserve [PORTNUM]", minus the quotes and  where PORTNUM
is some valid port number within the valid port number range that's not currently being used by someone else. The valid range 
for port numbers is 0-65535, but it's recommended to choose some number that's not a well-known port number, so maybe some number
in the range of 30000-65535. This will start chatserve on the host (flip1.engr.oregonstate.edu) on the specified port so it's
waiting for a connection. Now go to the other window (which should be on either flip2 or flip3, whichever you chose). Make sure you're
in the directory containing all of the program files. Type "./chatclient [HOSTNAME] [PORTNUM]", minus the quotes, where HOSTNAME is
the hostname used by chatserve (so here the hostname should be flip1.engr.oregonstate.edu) and PORTNUM is the same port number you
used when executing the chatserve program. At this point, in the chatclient window (so on flip2 or flip3, whichever you chose), you should
now be prompted to enter your desired user handle (the handle for the server has been hard-coded, as this was listed as an acceptable
option in the assignment instructions). Enter your client user handle and press enter. The chatclient program should then show your
handle and an arrow, a space, and then wait for your input. So it would look something like "[chatclientHandle]> ". Enter your first
message in the chatclient window here that you want to send to the server and press enter. This will establish the connection between
chatserve and chatclient. Now go to the chatserve window (flip1). You should see the prepended handle and the client's message printed
there, and on the next line you should see the hard-coded chatserve handle, an arrow, and a space, with the chatserve program now waiting
for your input. Enter the message you want to send back to the client from the server and press enter. Now go back to the chatclient window
(flip2 or flip3 whichever you chose). There, you should see the message sent by the server printed along with their prepended user handle,
and chatclient will be again waiting for your input to send to the server. You can continue sending messages back and forth in this way
in an alternating fashion, chatclient-chatserve-chatclient-chatserve-chatclient-chatserve for as long as you wish. Once you're done
messaging and are ready to close the connection between this client and the server, just type "\quit" (minus the quotes) as your message
on either the chatserve or chatclient windows. \quit will end the client connection regardless of whether you enter this \quit message
in the chatserve or chatclient windows. At that point, the connection with that chatclient and the chatserve will close, and the chatclient
program will end. The chatserve program, however, will continue running and waiting on its host and portnumber specified, waiting for
additional connections. If you want to establish another connection, repeat the execution of the chatclient program, using 
"./chatclient [HOSTNAME] [PORTNUM]", again where HOSTNAME is the host used for the server (flip1.engr.oregonstate.edu in this case) and
PORTNUM is whatever port number you used when executing chatserve. You can then enter your new handle in the chatclient window just
as before, send an initial message from there to the server, and then send messages back and forth in an alternating fashion
chatclient-chatserve-chatclient-chatserve for as long as you wish, again sending the message "\quit" whenever you want to close the
connection. You can do this entire sequence again for as many chatclients as you wish, as each time a chatclient connection is closed,
chatserve will continue to wait on the host and port number specified for additional connections. When you're completely done and ready
to terminate the chatserve program as well, just give it a SIGINT signal (press Ctrl-c), as this will terminate the chatserve program.
