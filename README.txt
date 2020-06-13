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
get the contents of the specified file), the filename is the file in the directory ftserver is in that you want to get
the contents of (again, onlyif you want a file, you don't have to include a filename if you just want the directory list),
and <dataPortNum> is the port number between 1025 and 65535 you want to use for the data connection. At this point, ftclient
and ftserver will interact accordingly, transferring the appropriate data, prompting you with messages if/when needed, etc.
If you want to test my input validation, you can of course enter the wrong number of args (i.e. neither 5 nor 6), invalid
commands, -l and a filename, -g but no filename, a filename that doesn't exist in ftserver's directory, a filename that exists in
ftserver's directory and is a duplicate that exists in ftclient's directory, etc. The programs should handle all of these scenarios. 
Once ftclient has terminated, ftserver will remain open on the original controlPortNum and will stay open for any additional
connections from ftclient you want to make, repeating the "./ftclient.py ...." pattern of inputs with whatever commands/filenames/
data port numbers you want to change. You just need to use the same serverPortNum whenever executing the ftclient.py script with
commands. You can continue in this fashion, where ftserver will stay open for accepting new connections when ftclient terminates,
and you can repeat calling the ./ftclient.py ... command as many times as you wish. Just make sure you're still in
the flip2.engr.oregonstate.edu window whenever you're executing the ./ftclient.py ... command. When you're really done and want
to terminate ftserver as well, go back to your flip1.engr.oregonstate.edu window, and type ctrl-c to terminate the process
w/ a SIGINT signal, as this will terminate ftserver. I have included 1 small, 1 med, and 1 large .txt file that I used successfully
for testing and were the file sizes as designated by the instructor on piazza, but you can use other .txt files if desired.

Note: when testing with a very large (~10MB) .txt file, the time you need to wait for ftserver to send all of the file contents
and ftclient to receive all of the file contents can be VERY long but it will send and receive it all due to the looping and end-
of-file character ftclient's recv() function knows to continue reading data until the EOF char has been found. Depending on flip
server traffic it sometimes took 30-40 minutes and occasionally over an hour for all of the file contents data requested
to be sent and received when the file was ~10MB. Please give plenty of time for this to occur and don't terminate the process
too early thinking there's been an error. My programs were tested with the files I included and though the process could be slow,
it did work for all of these file sizes and eventually print all messages described in the example execution provided in the assignment
instructions/requirements and in the rubric.

----------------------------------------------------------------------------------------------------------------------------------------

CS 372 Introduction to Computer Networks
Programming Assignment #2
Due Sunday, end of Week 9, by 11:59pm
Submit the source files, Makefile, and README in a .zip file to Canvas.
Objectives:
1. Implement 2-connection client-server network application
2. Practice using the sockets API
3. Refresh programming skills
The Program:
Design and implement a simple file transfer system, i.e., create a file transfer server and a file transfer client. Write the ftserver and the ftclient programs. The final version of your programs must accomplish the following tasks:
1. ftserver starts on Host A, and validates command-line parameters (<SERVER_PORT>).
2. ftserver waits on <PORTNUM> for a client request.
3. ftclient starts on Host B, and validates any pertinent command-line parameters. (<SERVER_HOST>, <SERVER_PORT>, <COMMAND>, <FILENAME>, <DATA_PORT>, etc…)
4. ftserver and ftclient establish a TCP control connection on <SERVER_PORT>. (For the remainder of this description, call this connection P)
5. ftserver waits on connection P for ftclient to send a command.
6. ftclient sends a command (-l (list) or -g <FILENAME> (get)) on connection P.
7. ftserver receives command on connection P.
If ftclient sent an invalid command
• ftserver sends an error message to ftclient on connection P, and ftclient displays the message on-screen.
otherwise
• ftserver initiates a TCP data connection with ftclient on <DATA_PORT>. (Call this connection Q)
• If ftclient has sent the -l command, ftserver sends its directory to ftclient on connection Q, and ftclient displays the directory on-screen.
• If ftclient has sent -g <FILENAME>, ftserver validates FILENAME, and either
- sends the contents of FILENAME on connection Q. ftclient saves the file in the current default directory (handling "duplicate file name" error if necessary), and displays a "transfer complete" message on-screen
or
- sends an appropriate error message (“File not found”, etc.) to ftclient on connection P, and ftclient displays the message on-screen.
• ftserver closes connection Q (don’t leave open sockets!).
8. ftclient closes connection P (don’t leave open sockets!) and terminates.
9. ftserver repeats from 2 (above) until terminated by a supervisor (SIGINT).
Page 2 of 3
Program Requirements:
• ftserver must be written in C/C++.
• ftclient must be written in Java or Python.
• Of course, your program must be well-modularized and well-documented.
• Your programs must run on a flip server: (flip1, flip2, flip3).engr.oregonstate.edu
o Probably the best way to do this is to use SSH Secure Shell, Putty, or another terminal emulator to log onto access.engr.oregonstate.edu using your ENGR username/password and note which flip you get.
o It will be easiest if you bring up two instances of the shell on the separate flip servers and use one to run the server, and the other to run the client (this is how I will be testing!).
• You may not use sendfile or any other predefined function that makes the problem trivial.
• Your program should be able to send a complete text file. You are not required to handle an “out of memory” error. Separate grading for short text files and long text files.
• Use the directories in which the programs are running. Don't hard-code any directories that might be inaccessible to the graders.
• Combine all program files into one *.zip archive (no .7z or .gz allowed). The .zip file should not contain any folders – only files!
• If you use additional include-files or make-files, be sure to include them in your .zip file.
• Create a README containing detailed instructions on how to compile and run your server and client.
• Be absolutely sure to cite any references and credit any collaborators. I’m sick of giving failing grades for people not doing this.
Options:
There are many possibilities for extra credit. All extra credit must be documented and referenced in your program description and README.txt to receive any credit. Here are a few ideas to get you started:
• Make your server multi-threaded.
• Implement username/password access to the server.
• Allow client to change directory on the server.
• Transfer files additional to text files (e.g. binary files) (a text file with a non-.txt extension doesn’t count.
• etc…
Page 3 of 3
Notes:
• Beej's Guide will be helpful. It has many things you’ll need for this assignment.
• Don't hard-code the port numbers
• Don’t use the well-known FTP port numbers, or 30021 or 30020, as these will be probably in use (by network services or other students).
• We will test your system with text files only (unless your README specifies additional file types), one very large and one small.
• If you implement extra credit features, be sure to fully describe those features, and how to use them, in your README, or you won’t receive any extra credit.
• Programs will be accepted up to 48 hours late with a 10% penalty per 24-hour period.
Example Execution:
SERVER (flip1)
CLIENT (flip2)
Input to console
Output
Input to Console
Output
> ftserver 30021
Server open on 30021
> ftclient flip1 30021 –l 30020
Connection from flip2.
List directory requested on port 30020.
Sending directory contents to flip2:30020
Receiving directory structure from flip1:30020
shortfile.txt
longfile.txt
> ftclient flip1 30021 –g shortfile.txt 30020
Connection from flip2.
File “shortfile.txt” requested on port 30020.
Sending “shortfile.txt” to flip2:30020
Receiving “shortfile.txt” from flip1:30020
File transfer complete.
> ftclient flip1 30021 –g longfileee.txt 30020
Connection from flip2.
File “longfileee.txt” requested on port 30020.
File not found. Sending error message to flip2:30021
flip1:30021 says FILE NOT FOUND
