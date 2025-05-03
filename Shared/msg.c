
/*------------------------------------------------------------

 Purpose: Contains Functions for Printing Error Messages
------------------------------------------------------------*/

#include "msg.h"

#include <stdio.h>

#include <unistd.h>

#include <time.h>

#include <string.h>

void printMsg(int msgCode)
{
	/*===========================================
	 STEP 1: Declaration of Variables
	===========================================*/

	printTime();

	switch (msgCode)
	{
	case 0:
		printf("Successfully carried out the operation\n");
		break;
	case RECV_OPCODE_ERR:
		printf("Failed to receive operation code.\n");
		break;
	case SEND_OPCODE_ERR:
		printf("Failed to send operation code.\n");
		break;

	case RECV_FILENAME_LENGTH_ERR:
		printf("Failed to receive file name length.\n");
		break;
	case SEND_FILENAME_LENGTH_ERR:
		printf("Failed to send file name length.\n");
		break;

	case RECV_FILENAME_ERR:
		printf("Failed to receive file name.\n");
		break;

	case SEND_FILENAME_ERR:
		printf("Failed to send file name.\n");
		break;

	case RECV_ACKCODE_ERR:
		printf("Failed to receive acknowledgement code.\n");
		break;
	case SEND_ACKCODE_ERR:
		printf("Failed to send acknowledgement code.\n");
		break;

	case RECV_FILE_SIZE_ERR:
		printf("Failed to receive file size.\n");
		break;
	case SEND_FILE_SIZE_ERR:
		printf("Failed to send file size.\n");
		break;

	case RECV_FILE_CONTENT_ERR:
		printf("Failed to receive file size.\n");
		break;
	case SEND_FILE_CONTENT_ERR:
		printf("Failed to send file content.\n");
		break;

	case RECV_DIRECTORY_PATH_ERR:
		printf("Failed to receive directory.\n");
		break;

	case SEND_DIRECTORY_PATH_ERR:
		printf("Failed to send directory.\n");
		break;

	case RECV_DIRECTORY_PATH_LENGTH_ERR:
		printf("Failed to receive directory length.\n");
		break;
	case SEND_DIRECTORY_PATH_LENGTH_ERR:
		printf("Failed to send directory length.\n");
		break;

	case OPEN_FILE_ERR:
		printf("Failed to open file.\n");
		break;
	case GET_CURRENT_WORKING_DIRECTORY_ERR:
		printf("Failed to get current working directory.\n");
		break;
	case CHANGE_DIRECTORY_ERR:
		printf("Failed to change directory.\n");
		break;
	case COMMAND_LINE_ARGUMENTS_ERR:
		printf("Invalid amount of command line argument \n");
		break;

	// Put ACK Code
	case SERVER_FILE_NAME_CLASH_ERR:
		printf("Server could not receive file due to name clash. \n");
		break;
	case SERVER_FILE_CREATE_ERR:
		printf("Server could not receive file due to failed in creating file. \n");
		break;
	case SERVER_OTHER_RECV_FILE_ERR:
		printf("Server could not receive file due to other reasons. \n");
		break;

	// Get ACK Code
	case SERVER_FILE_NOT_EXIST_ERR:
		printf("Server could not send file because file does not exist. \n");
		break;
	case SERVER_FILE_OPEN_ERR:
		printf("Server could not send file because file could not be opened. \n");
		break;
	case SERVER_OTHER_SEND_FILE_ERR:
		printf("Server could not send file because of other reasons. \n");
		break;

	case -SERVER_DIR_CREATE_FILE_ERR:
		printf("Server could not execute dir because failed to create a file for sending.");
		break;
	case -SERVER_DIR_OPEN_DIRECTORY_ERR:
		printf("Server could not execute dir because failed to open directory.");
		break;

	case IRREGULAR_FILE_ERR:
		printf("File is not a regular file.\n");
		break;

	case CLIENT_NOT_READY_ERR:
		printf("Server could not send file because client was not ready. \n");
		break;

	case FIND_FILE_CONTENT_ERR:
		printf("Failed to find file content.\n");
		break;

	case CONNECTION_ERR:
		printf("Connection lost.\n");
		break;

	case CDOPERATION_SUCCESS:
		printf("Successfully finished cd operation\n");
		break;

	case DIROPERATION_SUCCESS:
		printf("Successfully finished dir operation\n");
		break;

	case GETOPERATION_SUCCESS:
		printf("Successfully finished get operation\n");
		break;

	case PUTOPERATION_SUCCESS:
		printf("Successfully finished put operation\n");
		break;

	case PWDOPERATION_SUCCESS:
		printf("Successfully finished pwd operation\n");
		break;
	}

	printf("\n");

	fsync(STDOUT_FILENO);

	fsync(STDERR_FILENO);
}

void printTime()
{
	time_t rawtime;
	struct tm *timeinfo;

	char timeString[256];
	int timeStringSize;

	time(&rawtime);
	timeinfo = localtime(&rawtime);


	strcpy(timeString, ctime(&rawtime));
	timeStringSize = strlen(timeString);

	if (timeString[timeStringSize - 1] == '\n')
	{
		timeString[timeStringSize - 1] = '\0';
	}
	printf("[%s] ", timeString);
}
