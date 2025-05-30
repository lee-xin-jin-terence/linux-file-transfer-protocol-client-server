
/*------------------------------------------------------------

 Purpose: Contains Functions and Error Codes for 
          Printing Error Messages
------------------------------------------------------------*/

// CONNECTION / FTP - BASED ERR CODE

#define RECV_OPCODE_ERR -11

#define SEND_OPCODE_ERR -12

#define RECV_FILENAME_LENGTH_ERR -21

#define SEND_FILENAME_LENGTH_ERR -22

#define RECV_FILENAME_ERR -31

#define SEND_FILENAME_ERR -32

#define RECV_ACKCODE_ERR -41

#define SEND_ACKCODE_ERR -42

#define RECV_FILE_SIZE_ERR -51

#define SEND_FILE_SIZE_ERR -52

#define RECV_FILE_CONTENT_ERR -61

#define SEND_FILE_CONTENT_ERR -62

#define RECV_DIRECTORY_PATH_ERR -71

#define SEND_DIRECTORY_PATH_ERR -72

#define RECV_DIRECTORY_PATH_LENGTH_ERR -81

#define SEND_DIRECTORY_PATH_LENGTH_ERR -82

// CLIENT LOCAL ERR CODE

#define OPEN_FILE_ERR -101

#define GET_CURRENT_WORKING_DIRECTORY_ERR -201

#define CHANGE_DIRECTORY_ERR -301

#define COMMAND_LINE_ARGUMENTS_ERR -401

// SERVER ERR CODE

#define SERVER_FILE_NAME_CLASH_ERR -1001

#define SERVER_FILE_CREATE_ERR -1002

#define SERVER_OTHER_RECV_FILE_ERR -1003

#define SERVER_FILE_NOT_EXIST_ERR -2001

#define SERVER_FILE_OPEN_ERR -2002

#define SERVER_OTHER_SEND_FILE_ERR -2003

#define SERVER_DIR_CREATE_FILE_ERR -3001

#define SERVER_DIR_OPEN_DIRECTORY_ERR -3002

#define IRREGULAR_FILE_ERR -4001

#define CLIENT_NOT_READY_ERR -5001

#define FIND_FILE_CONTENT_ERR -6001

#define CONNECTION_ERR -7001

// SUCCESS CODE

#define CDOPERATION_SUCCESS 1

#define DIROPERATION_SUCCESS 2

#define GETOPERATION_SUCCESS 3

#define PUTOPERATION_SUCCESS 4

#define PWDOPERATION_SUCCESS 5





void printMsg(int msgCode);

void printTime();
