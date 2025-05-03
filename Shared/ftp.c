
/*------------------------------------------------------------

 Purpose: Contains Functions for String, File and Number
           Transfer using Sockets
------------------------------------------------------------*/

#include <sys/stat.h> // fstat()

#include <unistd.h> // read() and write()

#include <arpa/inet.h> // ntohs(), htons()

#include "ftp.h"

/* ##########################################################################################################################

	Reads from a file descriptor and store the data read in
	a buffer

	Return:
	   0 if Successful
	   -1 if Error occurred during reading
	
 ##########################################################################################################################*/

int readFromSockToBuffer(int socketFD, void *destBuffer, long sizeOfData)
{

	/*----------------------------------------------
	 STEP 1: Declaration of Variables
	-----------------------------------------------*/
	long totalNumOfBytesRead = 0;

	long numOfBytesRead = 0;

	/*----------------------------------------------------
	 STEP 2: Reading from socket and storing read data into
	 		the buffer
	----------------------------------------------------*/
	for (totalNumOfBytesRead = 0;
		 totalNumOfBytesRead < sizeOfData;
		 totalNumOfBytesRead += numOfBytesRead)
	{

		numOfBytesRead = read(socketFD,
							  (char *)destBuffer + totalNumOfBytesRead,
							  sizeOfData - totalNumOfBytesRead);

		/*---------------------------------------------------
		 STEP 3: If there is any error reading form the 
		 	socket, stop reading from the socket and return -1
		----------------------------------------------------*/
		if (numOfBytesRead <= 0)
		{
			return -1;
		}

	} //end of for-loop

	/*-----------------------------------------------------
	 STEP 3: If reading from socket is successful, return
		a value of 0
	-----------------------------------------------------*/
	return 0;
}

/* ##########################################################################################################################

  Write from a buffer to a file descriptor

  Return:
	 0 if successful
	 -1 if error occurred during writing	

 ########################################################################################################################## */

int writeFromBufferToSock(int socketFD, const void *srcBuffer,
						  long sizeOfData)
{
	/*----------------------------------------------
	 STEP 1: Declaration of Variables
	-----------------------------------------------*/

	long totalNumOfBytesWritten = 0;

	long numOfBytesWritten = 0;

	/*-----------------------------------------------------
	 STEP 2: Writing the data stored in the buffer into
	 		 the socket
	------------------------------------------------------*/
	for (totalNumOfBytesWritten = 0;
		 totalNumOfBytesWritten < sizeOfData;
		 totalNumOfBytesWritten += numOfBytesWritten)
	{

		numOfBytesWritten = write(socketFD,
								  (char *)srcBuffer + totalNumOfBytesWritten,
								  sizeOfData - totalNumOfBytesWritten);

		/*----------------------------------------------------
		 STEP 3: If there is any error writing to the socket,
		 	stop writing to the socket and return -1
		-----------------------------------------------------*/
		if (numOfBytesWritten <= 0)
		{
			return -1;
		}

	} //end of for-loop

	/*-----------------------------------------------
	 STEP 3: IF there is no error in writing to the
	  socket, return a value of 0
	------------------------------------------------*/
	return 0;
}

// ##########################################################################################################################

// SEND AND RECEIVE STRING SIZE

// ##########################################################################################################################

int sendStringSize(int socketFD, short stringSize)
{

	/*-----------------------------------------------------
	 STEP 1: Convert the string size to network byte order
	-----------------------------------------------------*/
	stringSize = htons(stringSize);

	/*-----------------------------------------------------
	 STEP 2: Write the string size to the socket. If there
	 is no error, return 0. Otherwise, return -1
	------------------------------------------------------*/
	return writeFromBufferToSock(socketFD, &stringSize, 2);
}

// ##########################################################################################################################

int receiveStringSize(int socketFD, short *stringSize)
{
	/*----------------------------------------------
	 STEP 1: Declaration of Variables
	-----------------------------------------------*/

	int receiveOutcome;

	/*----------------------------------------------
	 STEP 2: Read the string size from the socket
	-----------------------------------------------*/
	receiveOutcome = readFromSockToBuffer(socketFD, stringSize, 2);

	/*-----------------------------------------------
	 STEP 3: If there is no error receiving the 
	 	string size from the socket, convert the
		string size to a host-byte order
	-------------------------------------------------*/
	if (receiveOutcome != -1)
	{
		*stringSize = ntohs(*stringSize);
	}

	/*-------------------------------------------------
	 STEP 4: If there is no error in reading the 
	 string size from the socket, return 0. Otherwise,
	 return -1
	--------------------------------------------------*/
	return receiveOutcome;
}

// ##########################################################################################################################
// SEND AND RECEIVE STRING
// ##########################################################################################################################

int sendString(int socketFD, const char *stringToSend, short stringLength)
{
	/*-----------------------------------------------------
	 Return 0 if it successfully sends a string to the 
	 server, and -1 if it failes the send the string
	------------------------------------------------------*/
	return writeFromBufferToSock(socketFD, stringToSend, stringLength);
}

// ##########################################################################################################################

int receiveString(int socketFD, char *stringToReceive, short stringLength)
{
	/*----------------------------------------------
	 STEP 1: Declaration of Variables
	-----------------------------------------------*/
	int receiveStringOutcome;

	/*-----------------------------------------------
	 STEP 2: Read a string from the socket
	-------------------------------------------------*/
	receiveStringOutcome = readFromSockToBuffer(socketFD, stringToReceive, stringLength);

	if (receiveStringOutcome == 0)
	{
		stringToReceive[stringLength] = '\0';
	}

	/*---------------------------------------------------
	 STEP 3: Return 0 if there is no error receiving string.
	 		Otherwise, return -1
	---------------------------------------------------*/
	return receiveStringOutcome;
}

// ##########################################################################################################################

// SEND AND RECEIVE CODE

/*#########################################################################################################################
 
 Op Code:
 'P' - put operation.
 'G' - get operation.
 'W' - pwd operation.
 'D' - dir operation.
 'C' - cd operation.
 
 ACK Code:
 '0' - the server/client is ready to accept the named file
 '1' - the server/client cannot accept the file as there is a filename issue
 '2' - the server/client cannot accept the file because it cannot create the named file 
 '3' - the server cannot accept the file due to other reasons.

##########################################################################################################################*/

int sendCode(int socketFD, char code)
{

	/*--------------------------------------------------
	 Send a code to the socket. If there is no error,
	 return 0. Otherwise, return -1
	---------------------------------------------------*/
	return writeFromBufferToSock(socketFD, &code, sizeof(char));
	;
}

// ##########################################################################################################################

int receiveCode(int socketFD, char *code)
{
	/*-----------------------------------------------------
	 Receive a code from the socket. 

	 Returns 0 if there is no error receiving. Otherwise,
	 return -1
	-------------------------------------------------------*/

	return readFromSockToBuffer(socketFD, code, sizeof(char));
}

// ##########################################################################################################################

// SEND,RECEIVE AND FIND FILE SIZE

// ##########################################################################################################################

int sendFileSize(int socketFD, long fileSize)
{

	/*---------------------------------------------------
	 STEP 1: Convert the file size to network byte order
	----------------------------------------------------*/
	fileSize = htonl(fileSize);

	/*--------------------------------------------------
	 STEP 2: Send the file size to the socket. If there
	 		is no error sending, return 0. Otherwise,
			return -1.
	---------------------------------------------------*/
	return writeFromBufferToSock(socketFD, &fileSize, 8);
}

// ##########################################################################################################################

int receiveFileSize(int socketFD, long *fileSize)
{
	/*----------------------------------------------
	 STEP 1: Declaration of Variables
	-----------------------------------------------*/

	int readOutcome;

	readOutcome = readFromSockToBuffer(socketFD, fileSize, 8);

	/*-------------------------------------------------
	 STEP 2: If there is no error reading the file
	 		size, then convert the file size to 
			host byte order
	--------------------------------------------------*/
	if (readOutcome != -1)
	{
		*fileSize = ntohl(*fileSize);
	}

	/*-------------------------------------------------
	 STEP 3: Return 0 if there is no error reading the
	 		file size, and -1 if there is any error
	--------------------------------------------------*/
	return readOutcome;
}

// ##########################################################################################################################

long findFileSize(int fileFD)
{
	/*----------------------------------------------
	 STEP 1: Declaration of Variables
	-----------------------------------------------*/

	struct stat statBuf;

	long fileSize;

	/*-------------------------------------------------
	 STEP 2: Return the size of the file in bytes
	---------------------------------------------------*/

	fstat(fileFD, &statBuf);

	fileSize = statBuf.st_size;

	return fileSize;
}

/* ##########################################################################################################################
 
  Read from socket and write the data read
	into file

	0 if successful
	-1 if error reading from socket
	-2 if error writing to file

########################################################################################################################## */

int receiveFileContent(int socketFD, int destFileFD, long sizeOfData)
{
	/*----------------------------------------------
	 STEP 1: Declaration of Variables
	-----------------------------------------------*/

	char tempBuffer[MAX_BUFFER_SIZE];

	long numOfBytesRead = 0;

	long totalNumOfBytesRead = 0;

	long miniTotalNumOfBytesW = 0;

	long numOfBytesWritten = 0;

	/*--------------------------------------------------
	 STEP 2: Read data from the socket and write it into
	 		 a local file

			Return:
				- 1 if there is error reading from the
					socket
				-2 if there is error writing to a local
					file
	----------------------------------------------------*/
	for (totalNumOfBytesRead = 0;
		 totalNumOfBytesRead < sizeOfData;
		 totalNumOfBytesRead += numOfBytesRead)
	{

		numOfBytesRead = read(socketFD, tempBuffer,
							  MAX_BUFFER_SIZE);

		if (numOfBytesRead <= 0)
		{
			return -1;
		}

		for (miniTotalNumOfBytesW = 0;
			 miniTotalNumOfBytesW < numOfBytesRead;
			 miniTotalNumOfBytesW += numOfBytesWritten)
		{
			numOfBytesWritten = write(destFileFD,
									  tempBuffer + miniTotalNumOfBytesW,
									  numOfBytesRead - miniTotalNumOfBytesW);

			if (numOfBytesWritten <= 0)
			{
				//failed to write to destFileFD
				return -2;
			}

		} //end of inner for-loop

		if (numOfBytesWritten < numOfBytesRead)
		{
			return -2;
		}

	} //end of outer for-loop

	/*-----------------------------------------------
	 STEP 3: Return 0 if there is no error
	-----------------------------------------------*/
	return 0;
}

/* ##########################################################################################################################

   Return:
	0 of ok
	-1 if error reading from file
	-2 if error sending to socket

########################################################################################################################## */
int sendFileContent(int socketFD, int srcFileFD, long sizeOfData)
{
	/*----------------------------------------------
	 STEP 1: Declaration of Variables
	-----------------------------------------------*/

	char tempBuffer[MAX_BUFFER_SIZE];

	long numOfBytesWritten = 0;

	long miniTotalNumOfBytesW = 0;

	long totalNumOfBytesWritten = 0;

	long numOfBytesRead = 0;

	/*--------------------------------------------------
	 STEP 2: Read data from a local file and then
	 		send the file to the socket

		   Return:
		   		- 1 if there is any error reading from
				     a local file
				-2  if there is any error sending data
					 to the socket
	---------------------------------------------------*/
	for (totalNumOfBytesWritten = 0;
		 totalNumOfBytesWritten < sizeOfData;
		 totalNumOfBytesWritten += numOfBytesRead)
	{

		numOfBytesRead = read(srcFileFD, tempBuffer,
							  MAX_BUFFER_SIZE);

		if (numOfBytesRead <= 0)
		{
			//failed to read from srcFile
			return -1;
		}

		for (miniTotalNumOfBytesW = 0;
			 miniTotalNumOfBytesW < numOfBytesRead;
			 miniTotalNumOfBytesW += numOfBytesWritten)
		{

			numOfBytesWritten = write(socketFD,
									  tempBuffer + miniTotalNumOfBytesW,
									  numOfBytesRead - miniTotalNumOfBytesW);

			if (numOfBytesWritten <= 0)
			{
				//failed to write to socket
				return -2;
			}

		} //end of inner for-loop

	} //end of outer-for loop

	/*------------------------------------------
	 STEP 3: Return 0 if there is no error
	--------------------------------------------*/
	return 0;
}

// ##########################################################################################################################

int isRegFile(const char *fileName)
{
	/*----------------------------------------------
	 STEP 1: Declaration of Variables
	-----------------------------------------------*/

	struct stat statBuffer;

	int lstatRet;

	/*----------------------------------------------
	 STEP 2: Return 1 of file is a regular file,
	 	and 0 if otherwise
	-----------------------------------------------*/

	lstatRet = lstat(fileName, &statBuffer);

	if (lstatRet == -1)
	{
		return -1;
	}

	if (S_ISREG(statBuffer.st_mode))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
