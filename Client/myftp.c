
/*------------------------------------------------------------

 Purpose: The client program for the file transfer protocol
------------------------------------------------------------*/

#include <stdlib.h> // exit()

#include <sys/socket.h> // socket(), bind(), listen(), accept()

#include <strings.h> // bzero()

#include <netinet/in.h> // sockaddr_in

#include <netdb.h>

#include <fcntl.h> // open()

#include <sys/stat.h> // umask()

#include <sys/types.h> // pid_t

#include <stdio.h>

#include <arpa/inet.h> // for ntohs(), htons()

#include <string.h> // strcmp(), strlen()

#include <dirent.h> // opendir(), readdir()

#include <signal.h> // sigaction()

#include <unistd.h> // open(), unistd(), unlink(), chdir(),
					// setsid()

#include "../Shared/ftp.h"

#include "../Shared/msg.h"

#include "token.h" // tokenise()

#define tempTestSocketName "/tmp/testSocket"

//############################################################################################################

// START OF FUNCTION PROTOTYPES

//############################################################################################################

int cd(int socketFD, const char *directory);

int lcd(const char *directory);

int dir(int socketFD);

int ldir();

int pwd(int socketFD);

int lpwd();

int get(int socketFD, const char *fileName);

int put(int socketFD, const char *fileName);

int sendFileNameProcedure(int socketFD, const char *fileName, char opCode);

//############################################################################################################

// START OF MAIN

//############################################################################################################

int main(int argc, char *argv[])
{
	/*==================================================
	 STEP 1: Declaration of Variables
	===================================================*/

	u_long binaryInternetAddr;

	int socketFD;

	int connectRet;

	char userCommand[MAX_BUFFER_SIZE];

	char *tokenArr[MAX_BUFFER_SIZE];

	int userCommandLength;

	int operationStatus;

	int numOfTokens;

	struct sockaddr_in serverAddrStruct;

	struct hostent *hostentPtr;

	int ptonRet;

	char host[60];

	/*=================================================
	 STEP 2: Check if the number of arguments is correct
	==================================================*/
	if (argc == 1)
	{
		gethostname(host, sizeof(host));
	}
	else if (argc == 2)
	{
		strcpy(host, argv[1]);
	}
	else
	{
		printf("Usage: %s [ hostname | IP_address ] \n",
			   argv[0]);

		exit(1);
	}

	/*============================================
	 STEP 3: Check if the hostname is valid.

	 If the hostname is not valid, terminate the
	 program.
	=============================================*/
	hostentPtr = gethostbyname(host);

	if (hostentPtr != NULL)
	{
		binaryInternetAddr = *(u_long *)hostentPtr->h_addr;

	}
	else
	{
		ptonRet = inet_pton(AF_INET, host, &binaryInternetAddr);

		if (ptonRet != 1)
		{
			printf("Host/ip address %s not found\n", host);

			exit(1);
		}
	}

	
	

	
	/*==========================================
	STEP 4: Clear the umask and ignore the
			SIGPIPE signal

	 It is important to ignore the SIGPIPE
	 error so we can catch errors regarding
	 socket disconnection errors. Otherwise,
	 SIGPIPE if not ignored will cause the
	 program to terminate by default
	===========================================*/
	umask(0);

	signal(SIGPIPE, SIG_IGN);

	/*===========================================
	 STEP 5: Create a socket

	 If there is any error creating a socket,
	 terminate the program
	============================================*/

	socketFD = socket(AF_INET, SOCK_STREAM, 0);

	if (socketFD == -1)
	{
		perror("client, main: socket");

		exit(1);
	}

	/*===========================================
	 STEP 6: Connect to the server. 

	 If there is an error connecting to the 
	 server, terminate the program
	============================================*/

	bzero(&serverAddrStruct,
		  sizeof(serverAddrStruct));

	serverAddrStruct.sin_family = AF_INET;

	serverAddrStruct.sin_port = htons(TCP_SERVER_PORT);

	serverAddrStruct.sin_addr.s_addr = binaryInternetAddr;

	connectRet = connect(socketFD,
						 (struct sockaddr *)&serverAddrStruct,
						 sizeof(serverAddrStruct));

	if (connectRet == -1)
	{
		perror("client, main: connect");

		exit(1);
	}
	else
	{
		printf("Successfully connected to the server\n");
	}

	/*====================================================
	 STEP 7: Prompt and Get Commands

	 Depending on what the user enters, run the
	 command
	===================================================*/

	while (1)
	{

		/*========================================
		 STEP 7A: Prompt a command from the user
		=========================================*/
		printf("~$ ");

		fgets(userCommand, MAX_BUFFER_SIZE, stdin);

		userCommandLength = strlen(userCommand);

		if (userCommand[userCommandLength - 1] == '\n' && userCommandLength != 0)
		{
			userCommand[userCommandLength - 1] = '\0';
		}

		/*==================================================
		 STEP 7B: Tokenise the command that the user entered
		===================================================*/
		numOfTokens = tokenise(userCommand, tokenArr, MAX_BUFFER_SIZE);

		if (numOfTokens == 0)
		{
			printf("\nPlease enter a command.\n\n");
			continue;
		}

		/*===================================================
		 STEP 7C: Run the corresponding command that the
		 		  user entered
		====================================================*/

		/*==================================
		 7(c)(i)Command is lpwd (print local 
		 		working directory)
		===================================*/
		if (strcmp(tokenArr[0], "lpwd") == 0)
		{
			if (numOfTokens == 1)
			{
				operationStatus = lpwd();
			}
			else
			{
				operationStatus = COMMAND_LINE_ARGUMENTS_ERR;
			}
		}

		/*==================================
		 7(c)(ii)Command is pwd (print server
		 		 working directory)
		===================================*/
		else if (strcmp(tokenArr[0], "pwd") == 0)
		{
			if (numOfTokens == 1)
			{
				operationStatus = pwd(socketFD);
			}
			else
			{
				operationStatus = COMMAND_LINE_ARGUMENTS_ERR;
			}
		}

		/*==================================
		 7(c)(iii)Command is lcd (change 
		 		  local directory)
		===================================*/
		else if (strcmp(tokenArr[0], "lcd") == 0)
		{

			if (numOfTokens == 2)
			{
				operationStatus = lcd(tokenArr[1]);
			}
			else
			{

				operationStatus = COMMAND_LINE_ARGUMENTS_ERR;
			}
		}

		/*==================================
		 7(c)(iv)Command is cd (change 
		 		 server directory)
		===================================*/
		else if (strcmp(tokenArr[0], "cd") == 0)
		{

			if (numOfTokens == 2)
			{
				operationStatus = cd(socketFD, tokenArr[1]);
			}
			else
			{
				operationStatus = COMMAND_LINE_ARGUMENTS_ERR;
			}
		}

		/*==================================
		 7(c)(v)Command is ldir (print 
		        local directory PATHs)
		===================================*/
		else if (strcmp(tokenArr[0], "ldir") == 0)
		{
			if (numOfTokens == 1)
			{
				operationStatus = ldir();
			}
			else
			{
				operationStatus = COMMAND_LINE_ARGUMENTS_ERR;
			}
		}

		/*==================================
		 7(c)(vi)Command is cd (print 
		         server directory PATHs)
		===================================*/
		else if (strcmp(tokenArr[0], "dir") == 0)
		{
			if (numOfTokens == 1)
			{
				operationStatus = dir(socketFD);
			}
			else
			{
				operationStatus = COMMAND_LINE_ARGUMENTS_ERR;
			}
		}

		/*==================================
		 7(c)(vii)Command is get (get file 
		          from server)
		===================================*/
		else if (strcmp(tokenArr[0], "get") == 0)
		{

			if (numOfTokens == 2)
			{
				operationStatus = get(socketFD, tokenArr[1]);
			}
			else
			{
				operationStatus = COMMAND_LINE_ARGUMENTS_ERR;
			}
		}

		/*==================================
		 7(c)(viii)Command is put (put file 
		           to server)
		===================================*/
		else if (strcmp(tokenArr[0], "put") == 0)
		{

			if (numOfTokens == 2)
			{
				operationStatus = put(socketFD, tokenArr[1]);
			}
			else
			{
				operationStatus = COMMAND_LINE_ARGUMENTS_ERR;
			}
		}

		/*=================================
		 7(c)(ix)Command is quit (quit from 
		         the program)
		===================================*/

		else if (strcmp(tokenArr[0], "quit") == 0)
		{
			sendCode(socketFD, 'Q');
			
			close(socketFD);
			
			break;
		}

		else
		{
			printf("\nClient: Invalid Command entered!\n\n");
			continue;
		} // END OF IF-ELSE LADDER

		printMsg(operationStatus);

		if (operationStatus > -100 && operationStatus < 0)
		{
			printMsg(-7001);
			exit(1);
		}
		else if (operationStatus == -8001)
		{
			printf("Usage: %s <exactly one argument>.\n\n", tokenArr[0]);
		}

	} // END OF WHILE LOOP

	return 0;

} // END OF MAIN

//############################################################################################################

// FUNCTIONS



//############################################################################################################

// CD AND LCD

//############################################################################################################

int cd(int socketFD, const char *directory)
{
	/*==================================================
	 STEP 1: Declaration of Variables
	===================================================*/
	int sendOutcome;

	char serverReplyCode;

	int receiveCodeOutcome;

	short stringLength = strlen(directory);

	/*==================================================
	 STEP 2: Send operation code

	 If there is an error, return an error code
	===================================================*/
	sendOutcome = sendCode(socketFD, 'C');

	if (sendOutcome == -1)
	{
		return SEND_OPCODE_ERR;
	}

	/*==================================================
	 STEP 3: Send directory path length

	 If there is an error, return an error code
	===================================================*/
	sendOutcome = sendStringSize(socketFD, stringLength);

	if (sendOutcome == -1)
	{
		return SEND_DIRECTORY_PATH_LENGTH_ERR;
	}

	/*==================================================
	 STEP 4: Send directory path 

	 If there is an error, return an error code
	===================================================*/

	sendOutcome = sendString(socketFD, directory, stringLength);

	if (sendOutcome == -1)
	{
		return SEND_DIRECTORY_PATH_ERR;
	}

	/*==================================================
	 STEP 5: Receive server reply code

	 If there is an error, return an error code
	===================================================*/
	receiveCodeOutcome = receiveCode(socketFD, &serverReplyCode);

	if (receiveCodeOutcome == -1)
	{
		return RECV_ACKCODE_ERR;
	}

	if (serverReplyCode == '1')
	{
		return CHANGE_DIRECTORY_ERR;
	}

	/*==================================================
	 STEP 6: Return 0 if there is no error
	===================================================*/
	return 0;
}

//############################################################################################################

int lcd(const char *directory)
{
	/*==================================================
	 STEP 1: Declaration of Variables
	===================================================*/
	int chdirStatus;

	/*==================================================
	 STEP 2: Change directory based on directory path
	 
	 if there is an error, return an error code
	===================================================*/
	chdirStatus = chdir(directory);

	if (chdirStatus == -1)
	{
		return CHANGE_DIRECTORY_ERR;
	}

	/*==================================================
	 STEP 3: Return 0 if there is no error
	===================================================*/
	return 0;
}

//############################################################################################################

// DIR AND LDIR

//############################################################################################################

int dir(int socketFD)
{
	/*==================================================
	 STEP 1: Declaration of Variables
	===================================================*/
	long fileSize;

	int tempFileFD;

	char tempFileName[MAX_BUFFER_SIZE];

	char tempFileContentsBuffer[MAX_BUFFER_SIZE];

	int receiveOutcome;

	ssize_t numOfBytesRead;

	char serverReplyCode;

	/*===================================================
	 STEP 2: Create a temporary file

	 This temporary file will contain the list of 
	 files in the server directory

	 If there is an error opening the file, return with
	 an error code
	====================================================*/

	sprintf(tempFileName, "/tmp/tempDirFile%d", getpid());

	tempFileFD = open(tempFileName, O_CREAT | O_RDWR | O_TRUNC,
					  0777);

	if (tempFileFD == -1)
	{
		return OPEN_FILE_ERR;
	}

	/*======================================================
	 STEP 3: Send an operation code to the server informing
	 		 of dir operation

	 If there is an error sending the operation code, 
	 return with an error code
	=======================================================*/
	if (sendCode(socketFD, 'D') != 0)
	{
		close(tempFileFD);

		return SEND_OPCODE_ERR;
	}

	/*=====================================================
	 STEP 4: Receive a reply from the server

	 If there is an error receiving a reply, return with
	 an error code
	======================================================*/
	receiveOutcome = receiveCode(socketFD, &serverReplyCode);

	if (receiveOutcome == -1)
	{
		close(tempFileFD);

		return RECV_OPCODE_ERR;
	}

	/*======================================================
	 STEP 5: If the server reply with a non-ok reply code,
	 		which is any code that is not '0', return with
			 an error code
	=======================================================*/
	if (serverReplyCode == '1')
	{
		close(tempFileFD);

		return SERVER_DIR_CREATE_FILE_ERR;
	}

	if (serverReplyCode == '2')
	{
		close(tempFileFD);

		return SERVER_DIR_OPEN_DIRECTORY_ERR;
	}

	/*=====================================================
	 STEP 6: Receive the file size of temporary file 
	 		from the server containing all the files
			 in the server directory

	 If there is any error, return with an error code
	======================================================*/
	receiveOutcome = receiveFileSize(socketFD, &fileSize);

	if (receiveOutcome == -1)
	{
		close(tempFileFD);

		return RECV_FILE_SIZE_ERR;
	}

	/*=====================================================
	 STEP 7: Receive the file content from the server
	======================================================*/
	receiveOutcome = receiveFileContent(socketFD, tempFileFD,
										fileSize);

	if (receiveOutcome == -1)
	{
		close(tempFileFD);

		return RECV_FILE_CONTENT_ERR;
	}

	/*=====================================================
	 STEP 8: Print out the contents of the temp file
	 		 containing the list of files from the 
			  server directory
	======================================================*/

	lseek(tempFileFD, 0, SEEK_SET);

	printf("\n============================================================================ \n\n");

	while ((numOfBytesRead = read(tempFileFD, tempFileContentsBuffer, MAX_BUFFER_SIZE)) > 0)
	{
		tempFileContentsBuffer[numOfBytesRead] = '\0';
		printf("%s", tempFileContentsBuffer);
	}

	printf("============================================================================ \n");

	/*======================================================
	 STEP 9: Close and delete the temporary file
	=======================================================*/
	close(tempFileFD);

	unlink(tempFileName);

	return 0;
}

//############################################################################################################

int ldir()
{
	/*==================================================
	 STEP 1: Declaration of Variables
	===================================================*/
	DIR *dirPtr = NULL;

	struct dirent *direntPtr = NULL;

	int numOfFiles = 0;

	/*==================================================
	 STEP 2: Open the current directory

	 If there is error opening the current directory,
	 return with an error code
	===================================================*/
	dirPtr = opendir(".");

	if (dirPtr == NULL)
	{
		return GET_CURRENT_WORKING_DIRECTORY_ERR;
	}

	printf("\n==========================================="
		   "================================= \n\n");

	/*==================================================
	 STEP 3: Read and display the list of files in 
	 		 the current local directory
	===================================================*/
	while ((direntPtr = readdir(dirPtr)) != NULL)
	{

		if (strcmp(".", direntPtr->d_name) != 0 &&
			strcmp("..", direntPtr->d_name) != 0)
		{
			numOfFiles++;

			printf("%s\n", direntPtr->d_name);

			if ((numOfFiles % 5) == 0)
			{
				printf("\n");
			}
		}
	}

	printf("=========================================="
		   "================================== \n");

	/*=============================================
	 STEP 4: Return 0 if there is no error
	===============================================*/
	return 0;
}

//############################################################################################################

// PWD AND LPWD

//############################################################################################################

int pwd(int socketFD)
{
	/*==================================================
	 STEP 1: Declaration of Variables
	===================================================*/
	char serverWorkingDir[MAX_BUFFER_SIZE];

	short serverDirLength;

	int readServerDirOutcome;

	/*==================================================
	 STEP 2: Send operation code
	 
	 if there is an error, return an error code
	===================================================*/
	if (sendCode(socketFD, 'W') != 0)
	{
		return SEND_OPCODE_ERR;
	}

	/*==================================================
	 STEP 3: Read server directory path length
	 
	 if there is an error, return an error code
	===================================================*/
	readServerDirOutcome = receiveStringSize(socketFD, &serverDirLength);

	if (readServerDirOutcome == -1)
	{
		return RECV_DIRECTORY_PATH_LENGTH_ERR;
	}
	/*==================================================
	 STEP 4: Read server directory path 
	 
	 if there is an error, return an error code
	===================================================*/

	readServerDirOutcome = receiveString(socketFD, serverWorkingDir, serverDirLength);

	if (readServerDirOutcome == -1)
	{
		return RECV_DIRECTORY_PATH_ERR;
	}

	/*==================================================
	 STEP 5: Display server working directory 
	===================================================*/
	printf("\nServer working directory: %s\n", serverWorkingDir);

	/*==================================================
	 STEP 6: return 0 if there is no error 
	===================================================*/
	return 0;
}

//############################################################################################################

int lpwd()
{
	/*==================================================
	 STEP 1: Declaration of Variables
	===================================================*/
	char currDir[MAX_BUFFER_SIZE];

	/*==================================================
	 STEP 2: Get current working directory with getcwd

	 If there is an error, return an error code
	===================================================*/
	if (getcwd(currDir, MAX_BUFFER_SIZE) == NULL)
	{
		return GET_CURRENT_WORKING_DIRECTORY_ERR;
	}

	/*==================================================
	 STEP 3: Display current working directory 
	===================================================*/
	printf("\nLocal working directory: %s\n", currDir);

	/*==================================================
	 STEP 4: Return 0 if there is no error
	===================================================*/
	return 0;
}

//############################################################################################################

// GET AND PUT

//############################################################################################################

int get(int socketFD, const char *fileName)
{
	/*==================================================
	 STEP 1: Declaration of Variables
	===================================================*/
	int sendOutcome, receiveOutcome;

	int fileFD, fileNameOperationStatus;

	char ackCode;

	long fileSize;

	/*=====================================================
	 STEP 2: Send the opreation code,
	 		file name length and the file
	 		name to the server

	 If there is any error, return with an error code
	======================================================*/

	fileNameOperationStatus = sendFileNameProcedure(socketFD,
													fileName, 'G');

	if (fileNameOperationStatus != 0)
	{
		return fileNameOperationStatus;
	}

	/*====================================================
	 STEP 3: Receive an acknowledgement code from the
	 		server. If there is an error receiving the
			 acknowldegement code, return with
			 an an error code
	=====================================================*/
	receiveOutcome = receiveCode(socketFD, &ackCode);

	if (receiveOutcome == -1)
	{
		return RECV_ACKCODE_ERR;
	}

	/*====================================================
	 STEP 4: If the server acknowledgement code is not
	 ok, which is any code that is not '0', return with
	 an error code
	======================================================*/
	if (ackCode != '0')
	{
		switch (ackCode)
		{
		case '1':
			return SERVER_FILE_NOT_EXIST_ERR; // file not exist
		case '2':
			return SERVER_FILE_OPEN_ERR; // no permission
		case '3':
			return IRREGULAR_FILE_ERR; // not a regular file
		case '4':
			return SERVER_OTHER_SEND_FILE_ERR; // other reasons
		}
	}


	/*====================================================
	 STEP 5: Create a file to hold the incoming file data

	 If there is an error, send a non-ok reply to the
	 server and return with an error code.

	 Otherwise, send an ok reply('0') to the server
	=====================================================*/

	fileFD = open(fileName, O_CREAT | O_WRONLY | O_TRUNC,
				  0777);

	if (fileFD == -1)
	{
		sendOutcome = sendCode(socketFD, '1');
		return OPEN_FILE_ERR;
	}

	else
	{

		sendOutcome = sendCode(socketFD, '0');
	}

	if (sendOutcome == -1)
	{
		close(fileFD);

		unlink(fileName);

		return SEND_ACKCODE_ERR;
	}



	/*==================================================
	 STEP 6: Receive file size from server

	 if there is an error, return an error code
	===================================================*/
	receiveOutcome = receiveFileSize(socketFD, &fileSize);

	if (receiveOutcome != 0)
	{
		close(fileFD);

		unlink(fileName);

		return RECV_FILE_SIZE_ERR;
	}



	/*==================================================
	 STEP 7: Receive file content from server

	 if there is an error, return an error code
	===================================================*/
	receiveOutcome = receiveFileContent(socketFD, fileFD, fileSize);

	if (receiveOutcome != 0)
	{
		close(fileFD);

		unlink(fileName);

		return RECV_FILE_CONTENT_ERR;
	}


	/*==================================================
	 STEP 8: Return 0 if there is no error
	===================================================*/
	return 0;
}

//############################################################################################################

int put(int socketFD, const char *fileName)
{
	/*==================================================
	 STEP 1: Declaration of Variables
	===================================================*/
	int sendOutcome, receiveOutcome;

	int fileFD, fileOperationStatus;

	char ackCode;

	long fileSize;

	/*====================================================
	 STEP 2: Open the file to be sent across the socket

	 If there is an error opening the file, return with
	 an error code
	====================================================*/
	fileFD = open(fileName, O_RDONLY);

	if (fileFD == -1)
	{
		return OPEN_FILE_ERR;
	}

	/*===================================================
	 STEP 3: If the file is not a regular file, return
	 		with an error code
	======================================================*/
	if (isRegFile(fileName) == 0)
	{
		return IRREGULAR_FILE_ERR;
	}

	/*=====================================================
	 STEP 4: Send the operation code, file name length
	 		 and the file name to the server.

	 If there is any error, return with an error code
	======================================================*/
	fileOperationStatus = sendFileNameProcedure(socketFD,
												fileName, 'P');

	if (fileOperationStatus != 0)
	{
		return fileOperationStatus;
	}

	/*==================================================
	 STEP 5: Receive a reply from the server to see
	 		whether is ready

	 If there is error receiving the server reply, then
	 return with an error code
	===================================================*/
	receiveOutcome = receiveCode(socketFD, &ackCode);

	if (receiveOutcome == -1)
	{
		return RECV_ACKCODE_ERR;
	}

	/*===================================================
	 STEP 6: If the server reply code is not ok, which
	 		 any code that is not '0', return with an 
			  error code
	====================================================*/
	if (ackCode != '0')
	{
		switch (ackCode)
		{
		case '2':
			return SERVER_FILE_CREATE_ERR; //
		case '3':
			return IRREGULAR_FILE_ERR; //
		case '4':
			return SERVER_OTHER_RECV_FILE_ERR; //
		}
	}

	/*==================================================
	 STEP 7: Send file size to server

	 if there is an error, return an error code
	===================================================*/

	fileSize = findFileSize(fileFD);

	sendOutcome = sendFileSize(socketFD, fileSize);

	if (sendOutcome == -1)
	{
		return RECV_FILE_SIZE_ERR;
	}

	/*==================================================
	 STEP 8: Send file content to server

	 if there is an error, return an error code
	===================================================*/

	sendOutcome = sendFileContent(socketFD, fileFD, fileSize);

	if (sendOutcome == -1)
	{
		return FIND_FILE_CONTENT_ERR;
	}
	else if (sendOutcome == -2)
	{
		return SEND_FILE_CONTENT_ERR;
	}

	/*==================================================
	 STEP 9: Return 0 if there is no error
	===================================================*/
	return 0;
}

//###############################################################################

// Send file name

//###############################################################################
int sendFileNameProcedure(int socketFD, const char *fileName, char opCode)
{
	/*==================================================
	 STEP 1: Declaration of Variables
	===================================================*/
	int sendOutcome;

	/*==================================================
	 STEP 2: Send operation code

	 If there is an error, return an error code
	===================================================*/
	sendOutcome = sendCode(socketFD, opCode);

	if (sendOutcome == -1)
	{
		return SEND_OPCODE_ERR;
	}

	/*==================================================
	 STEP 3: Send string size

	 If there is an error, return with an error code
	===================================================*/
	sendOutcome = sendStringSize(socketFD, strlen(fileName));

	if (sendOutcome == -1)
	{
		return SEND_FILENAME_LENGTH_ERR;
	}

	/*======================================================
	 STEP 4: Send the actual string itself

	 If there is an error, return with an error code
	=======================================================*/
	sendOutcome = sendString(socketFD, fileName, strlen(fileName));

	if (sendOutcome == -1)
	{
		return SEND_FILENAME_ERR;
	}

	/*=======================================================
	 STEP 5: Return 0 if there is no error
	========================================================*/
	return 0;
}
