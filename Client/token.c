
/*---------------------------------------------------

 Author : Terence Lee Xin Jin, Hendra Kurniawan
----------------------------------------------------*/

#include "token.h"

#include <string.h>

//For NULL variable
#include <stddef.h>

int tokenise(char *line, char *tokenArr[],
			 int tokenArrCapacity)
{

	/*==========================================
	 STEP 1: Declaration of Variables
	===========================================*/

	int numOfTokens = 0;

	char *currentToken;

	/*=========================================
	 STEP 2: Store the starting address of
		each token in 'tokenArr'


	 If the number of tokens exceeded the capacity,
	 of tokenArr, stop the loop
	===========================================*/

	currentToken = strtok(line, TOKEN_DELIMITER);

	tokenArr[numOfTokens] = currentToken;

	while (currentToken != NULL)
	{

		numOfTokens++;

		if (numOfTokens >= tokenArrCapacity)
		{
			numOfTokens = -1;

			break;
		}

		currentToken = strtok(NULL, TOKEN_DELIMITER);

		tokenArr[numOfTokens] = currentToken;
	}

	/*==============================================
	 STEP 3: Return the Number of tokens

	 'numOfTokens' will contain the number of tokens
	  stored in the array 'tokenArr'. However, if
	  the  number of tokens exceeded the function
	  limit, a value of -1 will be returned instead
	===============================================*/

	return numOfTokens;
}
