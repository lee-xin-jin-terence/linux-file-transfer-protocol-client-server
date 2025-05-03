
/*------------------------------------------------------------

 Purpose: The server program for file transfer protocol

------------------------------------------------------------*/

#include <sys/types.h> // pid_t, u_long

#include <signal.h> // sigaction()

#include <string.h> // strcmp(), strlen()

#include <strings.h> // bzero()

#include <stdlib.h> // exit()

#include <sys/socket.h> // socket(), bind(), listen(), accept()

#include <netinet/in.h> // sockaddr_in

#include <sys/stat.h> // umask()

#include <errno.h> // for errno variable

#include <sys/wait.h> // waitpid()

#include <dirent.h> // opendir(), readdir()

#include <fcntl.h> // open()

#include <stdio.h>

#include <arpa/inet.h> // for ntohs(), htons()

#include <unistd.h> // open(), unistd(), unlink(), chdir(),
					// setsid()

#include "../Shared/ftp.h"

#include "../Shared/msg.h"

void daemonInitialise(void);

void redirectOutputToLogFile(void);

void serveClient(int socketFD);

void claimChildren(int signalNum);

int cdOperation(int socketFD);

int dirOperation(int socketFD);

int pwdOperation(int socketFD);

int getOperation(int socketFD);

int putOperation(int socketFD);

int getCurrentWorkingDir(char *currDir);

int receiveFileNameProcedure(int socketFD, char *fileName);

void printLogEntry(const char *logMsg, void *msgObject, char dataType);

char *clientIP;

//------------------------------------------------------

int main(int argc, char *argv[])
{

	/*==================================================
	 STEP 1: Declaration of Variables
	===================================================*/

	int socketFD;

	int newSocketFD;

	int bindRet;

	pid_t pid;

	int listenRet;

	struct sockaddr_in serverAddrStruct;

	struct sockaddr_in clientaddr;

	socklen_t clientaddr_size = sizeof(clientaddr);

	char initialCurrentDirectory[MAX_BUFFER_SIZE];

	/*=================================================
	 STEP 2: Check if the number of arguments is correct
	==================================================*/

	if (argc == 1)
	{
		strcpy(initialCurrentDirectory, ".");
	}
	else if (argc == 2)
	{
		strcpy(initialCurrentDirectory, argv[1]);
	}
	else
	{
		printf("Usage: %s [initial_current_directory] \n",
			   argv[0]);

		exit(1);
	}

	if (chdir(initialCurrentDirectory) == -1)
	{
		printf("Failed to change path to '%s'\n",
			   initialCurrentDirectory);

		exit(1);
	}

	/*===================================================
	 STEP 3: Make the server become a daemon
	===================================================*/
	daemonInitialise();

	/*===================================================
	 STEP 4: Create a socket

	 Terminiate the program if there is any 
	=====================================================*/
	socketFD = socket(AF_INET, SOCK_STREAM, 0);

	if (socketFD == -1)
	{
		perror("\nserver, main: socket");
		exit(1);
	}

	/*==================================================
	 STEP 5: Bind the server address to the socket

	 If there is any error, terminate the program
	===================================================*/
	bzero((char *)&serverAddrStruct,
		  sizeof(serverAddrStruct));

	serverAddrStruct.sin_family = AF_INET;

	serverAddrStruct.sin_port = htons(TCP_SERVER_PORT);

	serverAddrStruct.sin_addr.s_addr = htonl(INADDR_ANY);

	bindRet = bind(socketFD,
				   (struct sockaddr *)&serverAddrStruct,
				   sizeof(serverAddrStruct));

	if (bindRet == -1)
	{
		perror("\nserver,main: bind");

		exit(1);
	}


	/*==================================================
	 STEP 6: Redirect output to log file
	====================================================*/
	redirectOutputToLogFile();



	/*=====================================================
	 STEP 7: Listen for any connection
	======================================================*/

	listenRet = listen(socketFD, 5);

	if (listenRet == -1)
	{
		perror("\nserver, main: listen");

		exit(1);
	}

	/*=======================================================
	 STEP 7: Create a child server process to handle each
	 		client connection
	========================================================*/
	while (1)
	{

		newSocketFD = accept(socketFD, (struct sockaddr *)&clientaddr, &clientaddr_size);

		if (newSocketFD == -1)
		{
			/*============================================
			 If the error is due to interruption, try
			 to accept again
			============================================*/
			if (errno == EINTR)
			{
				continue;
			}

			perror("\nserver,main: accept");
			exit(1);
		}

		pid = fork();

		if (pid == -1)
		{
			perror("\nserver, main: fork");

			exit(1);
		}
		else if (pid > 0)
		{

			continue;
			close(newSocketFD);
		}
		else if (pid == 0)
		{
			close(socketFD);

			printf("\nServer:Now serving client");

			serveClient(newSocketFD);

			printf("\nServer: child exited as client closed "
				   "connection");

			exit(0);
		}

	} //end of while(1)

	return 0;
}

// ###############################################################################################################################

// DAEMON INITIALIZE

// ###############################################################################################################################

void daemonInitialise(void)
{

	/*======================================================
	 STEP 1: Declaration of Variables
	======================================================*/
	pid_t pid;

	struct sigaction sigactionStruct;



	/*========================================
	 STEP 2: Make the server become a daemon
	=========================================*/

	if ((pid = fork()) == -1)
	{
		perror("server, daemon_init: fork");
		exit(1);
	}
	else if (pid > 0)
	{
		exit(0);
	}

	/*======================================
	 STEP 3: Make the current child process
	 the process leader + clear the umask
	=======================================*/
	setsid();
	umask(0);

	/*=======================================
	 STEP 4: Create a handler for SIGCHLD
	=======================================*/

	sigactionStruct.sa_handler = claimChildren;

	sigactionStruct.sa_flags = SA_NOCLDSTOP;

	//not block other signals
	sigemptyset(&sigactionStruct.sa_mask);

	if (sigaction(SIGCHLD, &sigactionStruct, NULL) == -1)
	{
		perror("server,daemon_init: sigaction");

		exit(1);
	}
}


/*----------------------------------------------------------*/

void redirectOutputToLogFile()
{

	/*======================================================
	 STEP 1: Declaration of Variables
	======================================================*/

	const char fileLogName[MAX_BUFFER_SIZE] = "serverLog.txt";

	int fileLogFD;

	int dup2Outcome;


	/*=======================================================
	 STEP 2: Open the log file

	 Terminate the program if there is any error
	========================================================*/

	fileLogFD = open(fileLogName, O_CREAT | O_WRONLY | O_TRUNC, 0777);

	if (fileLogFD == -1)
	{
		perror("server , daemon_init: log");
		exit(1);
	}

	/* ======================================================
	 STEP 2: Redirect the output to the log file
	=======================================================*/

	dup2Outcome = dup2(fileLogFD, STDOUT_FILENO);

	if (dup2Outcome == -1)
	{
		perror("server , daemon_init: dup2 stdout");
		exit(1);
	}

	dup2Outcome = dup2(fileLogFD, STDERR_FILENO);

	if (dup2Outcome == -1)
	{
		perror("server , daemon_init: dup2 stderr");
		exit(1);
	}

}


// ###############################################################################################################################

// SERVE CLIENT

// ###############################################################################################################################

void serveClient(int socketFD)
{
	/*=============================================
	 STEP 1: Declaration of Variables
	-==============================================*/

	int receiveOutcome, operationStatus = -1, gpnStatus;

	char opCode;

	struct sockaddr_in clientAddrStruct;

	socklen_t clientAddrLength = sizeof(clientAddrStruct);

	/*============================================
	 STEP 2: Clear the umask of the client process
	==============================================*/
	umask(0);

	/*============================================
	STEP 3: Ignore the SIGPIPE signal

	This is important so that we can catch the
	error of writing to the socket when the client
	disconnects. Otherwise, the child server
	process would abruptly by default
	=============================================*/
	signal(SIGPIPE, SIG_IGN);

	/*============================================
	 STEP 4: Get the IP address of the client
	=============================================*/
	gpnStatus = getpeername(socketFD,
							(struct sockaddr *)&clientAddrStruct,
							&clientAddrLength);

	if (gpnStatus == -1)
	{
		printf("\nCPID %d: Unable to get the client address\n",
			   getpid());

		exit(1);
	}
	else
	{
		clientIP = inet_ntoa(clientAddrStruct.sin_addr);
	}
	printf("\nNow serving client\n\n");

	/*============================================
	 STEP 5: Listen to client requests
	============================================*/
	while (1)
	{

		receiveOutcome = receiveCode(socketFD, &opCode);

		if (receiveOutcome == -1)
		{
			printf("\nServer: child exited as client closed "
				   "connection\n");

			exit(1);
		}

		// ###############################################################################################################################

		printLogEntry("Client's operation code is ", &opCode, 'c');

		switch (opCode)
		{
		case 'C':
			operationStatus = cdOperation(socketFD);
			break;

		case 'D':
			operationStatus = dirOperation(socketFD);
			break;

		case 'G':
			operationStatus = getOperation(socketFD);
			break;

		case 'P':
			operationStatus = putOperation(socketFD);
			break;

		case 'W':
			operationStatus = pwdOperation(socketFD);
			break;

		case 'Q':
			exit(0);

		} // end of switch case

		printf("[%s]", clientIP);
		printMsg(operationStatus);


		/*================================================
		 If any of the above operation ended with a 
		 connection-related issue, where it failed to
		 receive or send anything from/ to the client,
		 it will terminate immediately
		==================================================*/
		if (operationStatus > -100 && operationStatus < 0)
		{
			printMsg(-7001);
			exit(1);
		}
	} // end of while(1)

} // end of function serveClient

//#####################################################

void claimChildren(int signalNum)
{
	/*----------------------------------------------
	 STEP 1: Declaration of Variables
	-----------------------------------------------*/

	int num;

	while (waitpid(0, &num, WNOHANG) > 0)
	{
		fprintf(stderr, "server: child exit status "
						"is %d\n",
				WEXITSTATUS(num));
	}
}

// ###############################################################################################################################

// CD OPERATION

// ###############################################################################################################################

int cdOperation(int socketFD)
{
	/*===============================================
	 STEP 1: Declaration of Variables
	=================================================*/

	int receiveOutcome, sendOutcome;

	char serverChangeDir[MAX_BUFFER_SIZE] = "";

	short serverChangeDirLength;

	/*===============================================
	 STEP 2: Receive new directory length

	 if there is an error receiving the length, 
	 return an error code
	=================================================*/

	receiveOutcome = receiveStringSize(socketFD, &serverChangeDirLength);

	printLogEntry("Server receiving directory path length ", NULL, 'n');

	if (receiveOutcome == -1)
	{
		return RECV_DIRECTORY_PATH_LENGTH_ERR;
	}

	printLogEntry("Directory path length: ", &serverChangeDirLength, 'i');

	/*===============================================
	 STEP 3: Receive new directory path 

	 if there is an error receiving the path, 
	 return an error code
	=================================================*/
	receiveOutcome = receiveString(socketFD, serverChangeDir, serverChangeDirLength);

	printLogEntry("Server receiving directory path ", NULL, 'n');

	if (receiveOutcome == -1)
	{
		return RECV_DIRECTORY_PATH_ERR;
	}

	printLogEntry("Directory path: ", serverChangeDir, 's');

	/*===============================================
	 STEP 4: If there is no error changing the 
	 		server working directory, reply the
			 client with an ok ('0')

			Otherwise, if there is an error changing,
			reply the client with a not-ok reply
	=================================================*/

	if (chdir(serverChangeDir) == 0)
	{
		sendOutcome = sendCode(socketFD, '0');

		printLogEntry("Server sending acknowledgement code ", NULL, 'n');

		if(sendOutcome == -1)
		{
			return SEND_ACKCODE_ERR;
		}
		else
		{
			printLogEntry("Server successfully sent acknowledgement code ", NULL, 'n');
		}
		
		
	}
	else
	{
		sendOutcome = sendCode(socketFD, '1');

		printLogEntry("Server sending acknowledgement code ", NULL, 'n');

		if(sendOutcome == -1)
		{
			return SEND_ACKCODE_ERR;
		}
		
		printLogEntry("Server successfully sent acknowledgement code ", NULL, 'n');
		
		return CHANGE_DIRECTORY_ERR;
		
	}

	printLogEntry("Server change directory success ", NULL, 'n');

	/*============================================
	 STEP 5: If there is an error sending the
	 		reply to the client, return with an
			error code
	==============================================*/
	if (sendOutcome == -1)
	{
		return SEND_ACKCODE_ERR;
	}

	printLogEntry("Server successfully sent acknowledgement code.", NULL, 'n');

	/*===============================================
	 STEP 6: return 1 if there is no error
	=================================================*/

	return CDOPERATION_SUCCESS;

} // END OF CD OPERATION

//#################################################

// DIR OPERATION

//##################################################
int dirOperation(int socketFD)
{
	/*==============================================
	 STEP 1: Declaration of Variables
	===============================================*/

	long fileSize;

	char tempFileName[MAX_BUFFER_SIZE];

	char *direntFileName;

	DIR *dirPtr = NULL;

	struct dirent *direntPtr = NULL;

	int numOfFiles = 0, sendOutcome, tempFileFD;

	/*================================================
	 STEP 2: Create a temporary file

	 If there is any error creating the temporary
	 file, return with an error code.Then, send a
	 non-ok reply to the server
	==================================================*/

	sprintf(tempFileName, "/tmp/tempDirFile%d", getpid());

	tempFileFD = open(tempFileName, O_CREAT | O_RDWR | O_TRUNC,
					  0777);

	printLogEntry("Server creating file ", tempFileName, 's');

	if (tempFileFD == -1)
	{
		printLogEntry("Server failed to create file ", tempFileName, 's');
		
		sendOutcome=sendCode(socketFD, '1');
		
		printLogEntry("Server sending acknowledgement code ", NULL, 'n');

		if(sendOutcome == -1)
		{
			return SEND_ACKCODE_ERR;
		}

		printLogEntry("Server successfully sent acknowledgement code ", NULL, 'n');

		return OPEN_FILE_ERR;
		

	}

	printLogEntry("Successfully opened file ", tempFileName, 's');

	/*====================================================
	 STEP 3: Open the current directory

	 If there is an error opening the current directory,
	 reply the client with an non-ok reply
	=====================================================*/
	dirPtr = opendir(".");

	printLogEntry("Server opening current directory ", NULL, 'n');

	if (dirPtr == NULL)
	{
		sendOutcome=sendCode(socketFD, '2');

		printLogEntry("Server sending acknowledgement code ", NULL, 'n');

		if(sendOutcome == -1)
		{
			return SEND_ACKCODE_ERR;
		}

		printLogEntry("Server successfully sent acknowledgement code ", NULL, 'n');
		
		return GET_CURRENT_WORKING_DIRECTORY_ERR;
	}

	printLogEntry("Server successfully opened current directory ", NULL, 'n');

	/*=====================================================
	 STEP 4: If there is no error, send the ok reply 
	 		to the client.

	 If there is an error sending an ok-reply to client,
	 return with a negative code
	======================================================*/

	sendOutcome = sendCode(socketFD, '0') ;
	
	printLogEntry("Server sending acknowledgement code ", NULL, 'n');

	if(sendOutcome == -1)
	{
		return SEND_ACKCODE_ERR;
	}

	printLogEntry("Server successfully sent acknowledgement code ", NULL, 'n');
	

	/*=================================================
	 STEP 5: Write the list of file names into the
	 		temporary file
	==================================================*/

	printLogEntry("Server currently writing to temporary file ", NULL, 'n');
	
	while ((direntPtr = readdir(dirPtr)) != NULL)
	{
		direntFileName = direntPtr->d_name;

		if (strcmp(".", direntFileName) != 0 &&
			strcmp("..", direntFileName) != 0)
		{
			numOfFiles++;

			write(tempFileFD, direntFileName, strlen(direntFileName));

			write(tempFileFD, "\n", 1);

			if ((numOfFiles % 5) == 0)
			{
				write(tempFileFD, "\n", 1);
			}
		}
	}

	fsync(tempFileFD);

	printLogEntry("Server finished writing to temporary file ", NULL, 'n');

	/*=====================================================
	 STEP 6: Get the file size of the temporary file created
	 		containing the list of files in the server
			 current working directory
	=====================================================*/

	printLogEntry("Server retrieving the file size of temporary file", NULL , 'n');	
	
	fileSize = findFileSize(tempFileFD);

	printLogEntry("File size: ", &fileSize, 'l');

	/*====================================================
	 STEP 7: Send the file size of the temporary file created
	  		containing the list of files in the server
			  current working directory

	 If there is any error, return with an error code
	=====================================================*/

	sendOutcome = sendFileSize(socketFD, fileSize);

	printLogEntry("Server sending the file size ", NULL, 'n');

	if (sendOutcome == -1)
	{
		close(tempFileFD);

		unlink(tempFileName);

		printLogEntry("Server deleted temporary file ", tempFileName, 's');

		return SEND_FILE_SIZE_ERR;
	}

	printLogEntry("Server successfully sent the file size ", NULL, 'n');

	/*=====================================================
	 STEP 8: Send the temp file to the client

	 If there is any error sending the file, return with
	 an error code
	======================================================*/
	lseek(tempFileFD, 0, SEEK_SET);

	sendOutcome = sendFileContent(socketFD, tempFileFD, fileSize);

	printLogEntry("Server sending the file content ", NULL, 'n');


	close(tempFileFD);

	unlink(tempFileName);

	printLogEntry("Server deleted temporary file ", tempFileName, 's');


	if (sendOutcome == -1)
	{
	
		//error reading from temp file
		return FIND_FILE_CONTENT_ERR;
	}
	else if (sendOutcome == -2)
	{
		//error sending temp file
		return SEND_FILE_CONTENT_ERR;
	}


	printLogEntry("Server successfully sent the file content ", NULL, 'n');
	

	return DIROPERATION_SUCCESS;

} // END OF DIR OPERATION

// ###############################################################################################################################

// GET

// ###############################################################################################################################

int getOperation(int socketFD)
{
	/*-============================================
	 STEP 1: Declaration of Variables
	==============================================*/
	int operationOutcome, fileFD, sendOutcome, receiveOutcome;

	char fileName[MAX_BUFFER_SIZE];

	long fileSize;

	char ackCode;

	/*========================================================
	 STEP 2: Receive the file name and the file name length
	 		 from the client

	  If there is any error, return with an error code
	=========================================================*/
	operationOutcome = receiveFileNameProcedure(socketFD, fileName);

	if (operationOutcome != 0)
	{
		return operationOutcome;
	}

	/*=======================================================
	 STEP 3: Open the file

	 If there is any error, send a not-ok reply to the 
	 client. Then return with a error code.
	========================================================*/
	fileFD = open(fileName, O_RDONLY);

	printLogEntry("Server opening file ", fileName , 's');

	if (fileFD == -1)
	{
		if (errno == ENOENT) //file not exists
		{
			sendOutcome=sendCode(socketFD, '1');
			
			printLogEntry("Server sending acknowledgement code ", NULL, 'n');

			if(sendOutcome == -1)
			{
				return SEND_ACKCODE_ERR;
			}
	
		}
		else if (errno == EACCES) //file cannot be opened
		{
			sendOutcome=sendCode(socketFD, '2');
			
			printLogEntry("Server sending acknowledgement code ", NULL, 'n');

			if(sendOutcome == -1)
			{
				return SEND_ACKCODE_ERR;
			}
		}
		else
		{
			sendOutcome=sendCode(socketFD, '4');
			
			printLogEntry("Server sending acknowledgement code ", NULL, 'n');

			if(sendOutcome == -1)
			{
				return SEND_ACKCODE_ERR;
			}
		}

		printLogEntry("Server successfully sent acknowledgement code ", NULL , 'n');

		return OPEN_FILE_ERR;
	}

	/*=====================================================
	 STEP 4: If the file the client is trying to get is
	 not a regular file, send a not-ok reply to the client

	 Then return with an error code
	======================================================*/
	if (isRegFile(fileName) == 0) 
	{
		sendOutcome=sendCode(socketFD, '3');
		
		printLogEntry("Server sending acknowledgement code ", NULL, 'n');

		if(sendOutcome == -1)
		{
			return SEND_ACKCODE_ERR;
		}

		printLogEntry("Server successfully sent acknowledgement code ", NULL , 'n');

		return IRREGULAR_FILE_ERR;
	}

	/*======================================================
	 STEP 5: Send an ok-reply back to the client
	=======================================================*/
	sendOutcome = sendCode(socketFD, '0');

	printLogEntry("Server sending acknowledgement code ", NULL , 'n');

	if (sendOutcome == -1)
	{
		return SEND_ACKCODE_ERR;
	}

	printLogEntry("Server successfully sent acknowledgement code ", NULL , 'n');


	/*=================================================
	 STEP 6: Get a reply from the client to see if the
	 		 client is ready

	 If there is an error getting a reply from the 
	 client, return with an error code
	===================================================*/
	receiveOutcome = receiveCode(socketFD, &ackCode);

	printLogEntry("Server receiving acknowledgement code ", NULL , 'n');

	if (receiveOutcome == -1)
	{
		return RECV_ACKCODE_ERR;
	}

	printLogEntry("Acknowledgement code: ", &ackCode , 'c');


	/*====================================================
	 STEP 7: If the client is not ready, return with a
	 		error code
	=====================================================*/
	if (ackCode != '0') //client not ready
	{
		return CLIENT_NOT_READY_ERR;
	}

	/*====================================================
	 STEP 8: Send the file size to the client

	 If there is an error sending the file size, return
	 with an error code
	=====================================================*/

	printLogEntry("Server finding the file size ", NULL , 'n');

	fileSize = findFileSize(fileFD);

	printLogEntry("File size: ", &fileSize , 'l');

	sendOutcome = sendFileSize(socketFD, fileSize);

	printLogEntry("Server sending the file size ", NULL , 'n');

	if (sendOutcome == -1)
	{
		return SEND_FILE_SIZE_ERR;
	}

	printLogEntry("Server successfully sent the file size ", NULL , 'n');
	
	/*====================================================
	 STEP 9: Send the file content to the client

	 If there is any error, return with an error code
	=====================================================*/

	sendOutcome = sendFileContent(socketFD, fileFD, fileSize);

	printLogEntry("Server sending the file content ", NULL , 'n');

	if (sendOutcome == -1)
	{
		return FIND_FILE_CONTENT_ERR;
	}
	else if (sendOutcome == -2)
	{
		return SEND_FILE_CONTENT_ERR;
	}

	printLogEntry("Server successfully sent the file content ", NULL , 'n');

	/*=====================================================
	 STEP 10: Return 3 if the entire operation is successful
	=======================================================*/
	return GETOPERATION_SUCCESS;
} // END OF GET OPERATION

int putOperation(int socketFD)
{
	/*----------------------------------------------
	 STEP 1: Declaration of Variables
	-----------------------------------------------*/
	int operationOutcome, fileFD, sendOutcome, receiveOutcome;

	char fileName[MAX_BUFFER_SIZE];

	long fileSize;

	/*=====================================================
	 STEP 2: Receive the file name 
	 		 name from the client

	 If there is any error, return a negative status
	=====================================================*/
	operationOutcome = receiveFileNameProcedure(socketFD, fileName);

	if (operationOutcome != 0)
	{
		return operationOutcome;
	}

	/*======================================================
	 STEP 3: Create a file 

	 If there is any error creating a file, send a not-ok
	 code back to the client. Then, return with a error
	 code
	=======================================================*/

	fileFD = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0777);

	printLogEntry("Server opening file ", fileName , 's');

	if (fileFD == -1)
	{

		if (errno == EACCES)
		{
			if(sendCode(socketFD, '2') == -1)
			{
				printLogEntry("Server failed to send the acknowledgement code ", NULL , 'n');
			}

			printLogEntry("Server successfully sent the acknowledgement code ", NULL , 'n');

			return SERVER_FILE_CREATE_ERR;
		}
		else
		{
			if(sendCode(socketFD, '4') == -1)
			{
				printLogEntry("Server failed to send the acknowledgement code ", NULL , 'n');
			}

			printLogEntry("Server successfully sent the acknowledgement code ", NULL , 'n');

			return SERVER_OTHER_RECV_FILE_ERR;
		}
	}

	printLogEntry("Server successfully opened file ", fileName , 's');

	/*=====================================================
	 STEP 4: Send an ok-code to the client saying that the
	 		 server is ready

	 If there is any error sending the code, return with
	 an error code
	======================================================*/
	sendOutcome = sendCode(socketFD, '0');

	printLogEntry("Server sending the acknowledgement code ", NULL , 'n');

	if (sendOutcome == -1)
	{
		close(fileFD);

		unlink(fileName);

		return SEND_OPCODE_ERR;
	}

	printLogEntry("Server successfully sent the acknowledgement code ", NULL , 'n');


	/*===================================================
	 STEP 5: Receive the file size from the client

	 If there is any error receiving the file size,
	 return with a negative status
	=====================================================*/
	receiveOutcome = receiveFileSize(socketFD, &fileSize);

	printLogEntry("Server receiving the file size ", NULL , 'n');

	if (receiveOutcome == -1)
	{
		close(fileFD);

		unlink(fileName);

		return RECV_FILE_SIZE_ERR;
	}

	printLogEntry("File size: ", &fileSize , 'l');

	/*======================================================
	 STEP 6: Receive the file content from the client.

	 If there is any error receiving the file contentm
	 return with an error code
	=======================================================*/

	receiveOutcome = receiveFileContent(socketFD, fileFD, fileSize);

	printLogEntry("Server receiving the file content ", NULL , 'n');

	if (receiveOutcome == -1)
	{
		close(fileFD);

		unlink(fileName);

		return RECV_FILE_CONTENT_ERR;
	}

	printLogEntry("Server successfully receive the file content ", NULL , 'n');

	/*=======================================================
	 STEP 7: Return with an 4 if there is no error
	========================================================*/
	return PUTOPERATION_SUCCESS;
}

int pwdOperation(int socketFD)
{
	/*===============================================
	 STEP 1: Declaration of Variables
	=================================================*/

	int sendOutcome;

	char currServerDir[MAX_BUFFER_SIZE];

	short currServerDirLength;

	/*===============================================
	 STEP 2: Get the server current working directory
	=================================================*/
	getCurrentWorkingDir(currServerDir);

	/*===============================================
	 STEP 3: Send current working directory length
	 	     to client
	 
	 if there is an error in sending to client, 
	 return an error code
	=================================================*/

	currServerDirLength = strlen(currServerDir);

	sendOutcome = sendStringSize(socketFD, currServerDirLength);

	printLogEntry("Directory path length: ", &currServerDirLength , 'i');

	printLogEntry("Server sending directory path length ", NULL , 'n');

	if (sendOutcome == -1)
	{
		return SEND_DIRECTORY_PATH_LENGTH_ERR;
	}

	printLogEntry("Server successfully sent directory path length ", NULL , 'n');

	/*====================================================
	 STEP 4: Send the server working directory to the client

	 If there is any error, return with an error code
	====================================================*/
	sendOutcome = sendString(socketFD, currServerDir, currServerDirLength);

	printLogEntry("Directory path: ", currServerDir , 's');

	printLogEntry("Server sending directory path ", NULL , 'n');

	if (sendOutcome == -1)
	{
		return SEND_DIRECTORY_PATH_ERR;
	}

	printLogEntry("Server successfully sent directory path ", NULL , 'n');
	/*===============================================
	 STEP 5: return 5 if there is no error
	=================================================*/
	return PWDOPERATION_SUCCESS;

} // END OF PWD OPERATION

int getCurrentWorkingDir(char *currDir)
{
	/*===============================================
	 STEP 1: Get current working directory

	 if getcwd returns null, return an error code
	=================================================*/

	if (getcwd(currDir, MAX_BUFFER_SIZE) == NULL)
	{
		return GET_CURRENT_WORKING_DIRECTORY_ERR;
	}

	/*===============================================
	 STEP 2: Return 0 if there is no error
	=================================================*/
	return 0;

} // END OF GET CURRENT WORKING DIRECTORY

int receiveFileNameProcedure(int socketFD, char *fileName)
{
	/*====================================================
	 STEP 1: Declaration of variables
	=====================================================*/
	short fileNameLength;

	int receiveOutcome;

	/*=====================================================
	 STEP 2: Receive the size of the file name

	 If there is any error, return with an error code
	=====================================================*/
	receiveOutcome = receiveStringSize(socketFD, &fileNameLength);

	printLogEntry("Server receiving filename length", NULL, 'n');

	if (receiveOutcome == -1)
	{
		return RECV_FILENAME_LENGTH_ERR;
	}

	printLogEntry("Filename length: ", &fileNameLength , 'i');

	/*====================================================
	 STEP 3: Receive the file name

	 If there is any error, return with an error code
	=====================================================*/
	receiveOutcome = receiveString(socketFD, fileName, fileNameLength);

	printLogEntry("Server receiving filename", NULL, 'n');

	if (receiveOutcome == -1)
	{
		return RECV_FILENAME_ERR;
	}

	printLogEntry("Filename: ", fileName , 's');

	/*=====================================================
	 STEP 4: Return 0 if there is no error
	======================================================*/
	return 0;
}

//###############################################

void printLogEntry(const char *logMsg, void *msgObject, char dataType)
{

	/*=========================================
	 Print Client IP address, time and log
	 message to the log file
	==========================================*/
	printf("[%s]", clientIP);

	printTime();

	printf("%s", logMsg);

	switch (dataType)
	{
	//character
	case 'c':
		printf("%c\n", *(char *)(msgObject));
		break;

	//short
	case 'i':
		printf("%d\n", *(short *)(msgObject));
		break;

	//long
	case 'l':
		printf("%ld\n", *(long *)(msgObject));
		break;

	//string
	case 's':
		printf("%s\n", (char *)(msgObject));
		break;

	default:
		printf("\n");
		break;
	}
}
