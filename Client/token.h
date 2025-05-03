
/*--------------------------------------------------

 Author: Terence Lee Xin Jin, Hendra Kurniawan

---------------------------------------------------*/

#define TOKEN_DELIMITER " \t\n"

/*-------------------------------------------
	 Brief:
		Tokenise a string into an array of 
		tokens


	 Parameters:
		line - this is the input of the 
		   function. Note: This function modifies
		   the string 'line' by inserting the
		   null-terminating character '\0' after
		   each token into 'line'

		tokenArr - this is the output of 
		   the function. The starting address
		   of each token from 'line' will be
		   stored in this array


		tokenArrCapacity - this is the maximum
		   number of tokens that can be stored
		   in the array tokenArr
 
	 Return:
		int - the number of tokens stored
		  in the array 'tokenArr'
	--------------------------------------------*/

int tokenise(char *line, char *tokenArr[],
			 int tokenArrCapacity);
