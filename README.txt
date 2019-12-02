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
too early thinking there's been an error. It was tested with the files I included and though the process could be slow, it did
work for all of these file sizes.
