
/*------------------------------------------------------------

 Purpose: Contains Functions for String, File and Number
           Transfer using Sockets
------------------------------------------------------------*/

#define MAX_BUFFER_SIZE (1024 * 5) /* maximum size of any piece of */
                                   /* data that can be sent by client */

#define TCP_SERVER_PORT 40548

/*
 * purpose:  read a stream of bytes from "fd" to "buf".
 * pre:      1) size of buf bufsize >= MAX_BLOCK_SIZE,
 * post:     1) buf contains the byte stream; 
 *           2) return value > 0   : number ofbytes read
 *                           = 0   : connection closed
 *                           = -1  : read error
 *                           = -2  : protocol error
 *                           = -3  : buffer too small
 */

int sendStringSize(int socketFD, short stringSize);

int receiveStringSize(int socketFD, short *stringSize);

int sendString(int socketFD, const char *stringToSend, short stringLength);

int receiveString(int socketFD, char *stringToReceive, short stringLength);

int sendCode(int socketFD, char code);

int receiveCode(int socketFD, char *code);

int sendFileSize(int socketFD, long fileSize);

int receiveFileSize(int socketFD, long *fileSize);

/*
  -----------------------------------------------------
  Read from a socket to a file
    
  Return:
    0 if no errors
    -1 if error reading from socket
    -2 if error writing to file
  -----------------------------------------------------
*/
int receiveFileContent(int socketFD, int destFileFD, long sizeOfData);

/*-----------------------------------------------------
  Write from file to socket

 Return:
	0 if no error
	-1 if error reading from file
	-2 if error writing to socket
-------------------------------------------------------*/
int sendFileContent(int socketFD, int srcFileFD, long sizeOfData);

long findFileSize(int fileFD);

/*----------------------------------------------------------
 Check if a file is a regular file

 Parameters:
    fileName - name of the file of interest

 Return:
  -1 if file does not exists
  1 if file is a regular file
  0 if file is not a regular file
-----------------------------------------------------------*/
int isRegFile(const char *fileName);
