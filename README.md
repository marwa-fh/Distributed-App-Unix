# Distributed-App-Linux
developed a distributed application consisting of 2 parts: simple authentication protocol and a game between distributed clients.

# The app is composed of:
1. authenication server
2. Game Server
3. Distributed Clients

   The scenario is developed with C language using process, pipes, and forks..

# Requirnments
OS: linux e.g. : ubuntu/BOSS/parrot/garuda/linuxmint..

# Scenario
Since the application is composed of 3 components;
	We created 3 process using 2 forks
	 One for authentication server as a parent and 2 for Game Server and Distributed clients as a child


	To establish the connection (write & read data) between the components we used pipes, we needed 6 here, this requires 6 arrays of size 2, the first element (0) in the array is set up and opened for reading, while the second element (1) is set up and opened for writing.
	We closed the end of pipe we aren't concerned with

	Sleep () function was used to maintain synchronization 
	We developed a function to take the string between 2 “:”
	We supposed that we have 3 authorization messages that contains the users and their passwords in order to compare it with data taken from the clients since there is no database given
	Rand () used to generate random number for each round, sprint( , “%d”, ) to cast int to char[]
	Strcmp used to compare strings 



Problems faced in creating 3 clients without distributing the connections & messages between components
So, we generated 2 scenarios one with 3 clients that stops when “ReadyToStart” message is sent;
Other that works with 1 client until it receives 10 words from the client to predict the MysteryWord 
