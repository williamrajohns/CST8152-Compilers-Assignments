/*
* File Name: scanner.c
* Compiler: MS Visual Studio 2019
* Author: William Johns, 040 832 509
* Course: CST8152 - Compilers, Lab Section: 013
* Assignment: 2 - Scanner (A2)
* Date: 21st Nov 2020
* Proffesor/Original file from: Paulo Sousa
* Purpose: Contains the definitions for some the functions declared in table.h, as well as acts as the scanner for the buffer
*			Contains methods to parse the chars in the buffer and determine the correct tokens to assign these chars
*			Ultimately these tokens will be displayed from testScanner
* Function list: initScanner(), processToken(void), getNextState(), nextTokenClass(), aStateFuncAVID(), aStateFuncSVID(), aStateFuncIL(), aStateFuncFPL(), aStateFuncSL(), aStateFuncErr(), isKeyword()
*/



 /* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
  * to suppress the warnings about using "unsafe" functions like fopen()
  * and standard sting library functions defined in string.h.
  * The define does not have any effect in Borland compiler projects.
  */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

  /*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"

#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
   It is defined in platy_st.c */
extern pBuffer stringLiteralTable;		/* String literal table */
int line;								/* current line number of the source code */
extern int errorNumber;					/* defined in platy_st.c - run-time error number */

static char debugMode = 0;				/* optional for debugging */

/* Local(file) global objects - variables */
static pBuffer lexemeBuffer;			/* pointer to temporary lexeme buffer */
static pBuffer sourceBuffer;			/* pointer to input source buffer */
/* No other global variable declarations/definitiond are allowed */

/* scanner.c static(local) function  prototypes */
static int nextTokenClass(char c);		/* character class function */
static int getNextState(int, char);		/* state machine function */
static int isKeyword(char* kw_lexeme);	/* keywords lookup functuion */

/* TODO_02: Follow the standard and adjust all function headers */
/*
* Function Name: initScanner
* Purpose: Checks to see if the buffer is empty, if so returns an error if not sets things up so that the scanner can be used
* Author: William Johns
* Version: 1.0 November 21 2020
* Called Functions: bufferIsEmpty(), bufferRewind(), bufferClear()
* Parameters: pBuffer psc_buf, pointer to the buffer
* Return Value: int 1 if the buffer is empty
*				int 0 if sucessful in setting up things for the scanner
* Algorithm:
*/
int initScanner(pBuffer psc_buf) {
	if (bufferIsEmpty(psc_buf)) return EXIT_FAILURE;/*1*/
	/* in case the buffer has been read previously  */
	bufferRewind(psc_buf);
	bufferClear(stringLiteralTable);
	line = 1;
	sourceBuffer = psc_buf;
	return EXIT_SUCCESS;/*0*/
/*   scerrnum = 0;  *//*no need - global ANSI C */
}

/*
* Function Name: processToken
* Purpose: Goes through the buffer one char at a time and checks to see what token can be assigned to them.
* Author: William Johns
* Version: 1.0 November 21 2020
* Called Functions: bufferGetChar(), bufferRetract(), getNextState(), bufferGetCPosition(), bufferSetMarkPosition(), bufferCreate(), bufferFree(), bufferReset(),
*					aStateFuncAVID(), aStateFuncSVID(), aStateFuncIL(), aStateFuncFPL(), aStateFuncSL(), aStateFuncErr(),
*					strcpy(), strcat()
* Parameters: 
* Return Value: Token currentToken with the correct code and attributes attached to it
* Algorithm: First tries to see what the current char is using bufferGetChar for the source buffer
*			 Using a switch case statement to check if it is a special token
*			 If it is assigns the correct attibutes and code to the current token and returns it
*			 If it isn't a special token creates a temp buffer and sends it to the correct final state function to check if it is valid
*			 Before ultimately returning that token
*			 If an error occurs it should return an error token
*/
Token processToken(void) {
	Token currentToken = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c;	/* input symbol */
	int state = 0;		/* initial state of the FSM */
	short lexStart;		/* start offset of a lexeme in the input char buffer (array) */
	short lexEnd;		/* end offset of a lexeme in the input char buffer (array)*/

	int lexLength;		/* token length */
	int i;				/* counter */
	int isError;		/* error identification */
	unsigned char newc;	/* new char */

	while (1) { /* endless loop broken by token returns it will generate a warning */
		c = bufferGetChar(sourceBuffer);

		/* Part 1: Implementation of token driven scanner */
		/* every token is possessed by its own dedicated code */
		unsigned char tempChar; //added this
		switch (c) {
		case ' ': case '\t': case '\v': case '\f': case '\r': //whitespace chars excluding newline
			continue;
		case '\n': //newline has to increment new line, the rest repeat
			line++;
			continue;

		//comments
		case '!':
			newc = bufferGetChar(sourceBuffer); //get the next char 
			//if the next char is ! then it is a comment until the next line
				if (newc == '!') { 
				while (newc != '\n') {	//ignore chars until new line
					//get next char
					newc = bufferGetChar(sourceBuffer);

					if (newc == CHARSEOF0 || newc == CHARSEOF255) { //if eof retract 
						bufferRetract(sourceBuffer);	
						break;  //break while loop
					}
				}

				//add new line if there is one (don't if eof)	
				if (newc == '\n') {
					line++;
				}
				continue; //continue to next token
			}

			//not a comment 
			if (newc != '!') {
				currentToken.code = ERR_T; //assignt the correct error values
				currentToken.attribute.err_lex[0] = '!';
				tempChar = newc;

				//until new line
				while (newc != '\n') {
					//if it runs into SEOS 
					if (newc == CHARSEOF0 || newc == CHARSEOF255) {
						bufferRetract(sourceBuffer);
						break; //error token below
					}
					newc = bufferGetChar(sourceBuffer);

				}
				//increment new line if there is one
				if (newc == '\n') {
					line++;
				}

				//if the tempChar is not an SEOF/EOF char add it to the end of the err_lex
				if (tempChar != CHARSEOF0) {
					currentToken.attribute.err_lex[1] = tempChar;
					currentToken.attribute.err_lex[2] = CHARSEOF0;
				}
				else if (tempChar != CHARSEOF255) {
					currentToken.attribute.err_lex[1] = tempChar;
					currentToken.attribute.err_lex[2] = CHARSEOF255;
				}
				else
				{
					currentToken.attribute.err_lex[1] = CHARSEOF0;
				}
				//return error token
				return currentToken;
			}
		
		case '$': //string concat symbol 
			newc = bufferGetChar(sourceBuffer);
			if (newc == '$') { //is string concat
				currentToken.code = SCC_OP_T;
				return currentToken;
			}
			//implied else 
			//bufferRetract(sourceBuffer);
			
			bufferRetract(sourceBuffer);
			currentToken.code = ERR_T;//Set the correct error values
			strcpy(currentToken.attribute.err_lex, "$");
			return currentToken;

		//seperators 
		case '(':
			currentToken.code = LPR_T;
			return currentToken;
		case ')':
			currentToken.code = RPR_T;
			return currentToken;
		case '{':
			currentToken.code = LBR_T;
			return currentToken;
		case '}':
			currentToken.code = RBR_T;
			return currentToken;
		case ',':
			currentToken.code = COM_T;
			return currentToken;
		case ';':
			currentToken.code = EOS_T;
			return currentToken;

		//arithmetic operators
		case '+': 
			currentToken.code = ART_OP_T;
			currentToken.attribute.arr_op = ADD;
			return currentToken;
		case '-':
			currentToken.code = ART_OP_T;
			currentToken.attribute.arr_op = SUB;
			return currentToken;
		case '*':
			currentToken.code = ART_OP_T;
			currentToken.attribute.arr_op = MUL;
			return currentToken;
		case '/':
			currentToken.code = ART_OP_T;
			currentToken.attribute.arr_op = DIV;
			return currentToken;
		
		//relational operators 
		case '<': 
			newc = bufferGetChar(sourceBuffer);

			//case is <>
			if (newc == '>') {
				currentToken.code = REL_OP_T;
				currentToken.attribute.rel_op = NE;
				return currentToken;
			}
			//implied else (just a '<' ) 
			bufferRetract(sourceBuffer); //retract 1 (since it isnt <>)
			currentToken.code = REL_OP_T;
			currentToken.attribute.rel_op = LT;
			return currentToken;
		case '>':
			currentToken.code = REL_OP_T;
			currentToken.attribute.rel_op = GT;
			return currentToken;
		case '=': 
			newc = bufferGetChar(sourceBuffer);
			//if it would make ==
			if (newc == '=') {
				currentToken.code = REL_OP_T;
				currentToken.attribute.rel_op = EQ;
				return currentToken;
			}
			//implied else 
			bufferRetract(sourceBuffer); //retract since it isn't == 
			currentToken.code = ASS_OP_T;
			return currentToken;

		//logical operator
		case '_':  //redo this with temp buffer 
			newc = bufferGetChar(sourceBuffer);
			tempChar = bufferGetChar(sourceBuffer);

			//_AND_ 
			if (newc == 'A' && tempChar == 'N') { //if the next two chars would make _AN
				
				newc = bufferGetChar(sourceBuffer);
				tempChar = bufferGetChar(sourceBuffer);
				if (newc == 'D' && tempChar == '_') { //would make _AND_
					currentToken.code = LOG_OP_T;
					currentToken.attribute.log_op = AND;
					return currentToken;
				}
				//implied else, retract the last two chars
				bufferRetract(sourceBuffer);
				bufferRetract(sourceBuffer);
			}
			//_OR_ 
			else if (newc == 'O' && tempChar == 'R') //would make _OR
			{
				newc = bufferGetChar(sourceBuffer);
				if (newc == '_') { //would make _OR_
					currentToken.code = LOG_OP_T;
					currentToken.attribute.log_op = OR;
					return currentToken;
				}
				//implied else, retract the last char
				bufferRetract(sourceBuffer);
				
			}
			//_NOT_
			else if (newc == 'N' && tempChar == 'O') { //would make _NO
				newc = bufferGetChar(sourceBuffer);
				tempChar = bufferGetChar(sourceBuffer);
				if (newc == 'T' && tempChar == '_') { //would make _NOT_
					currentToken.code = LOG_OP_T;
					currentToken.attribute.log_op = NOT;
					return currentToken;
				}
				//implied else, retract the last two char
				bufferRetract(sourceBuffer);
				bufferRetract(sourceBuffer);
			}

			//implied else, not one of the tokens, ie its an error
			//retract back to original position

			bufferRetract(sourceBuffer);
			//bufferRetract(sourceBuffer);

			currentToken.code = ERR_T;	//set the correct error values
			strcpy(currentToken.attribute.err_lex, "_");
			return currentToken; 		//return error token
		
		//CHARSEOF0 & CHARSEOF255
		case CHARSEOF0:
			currentToken.code = SEOF_T;
			currentToken.attribute.seof = SEOF_0;
			return currentToken;
		case CHARSEOF255:
			currentToken.code = SEOF_T;
			currentToken.attribute.seof = SEOF_EOF;
			return currentToken;

		//error case with char c = bufferGetChar(sourceBuffer);
		case RT_FAIL_1: case RT_FAIL_2:
			
			errorNumber = RT_FAIL_2;
			strcpy(currentToken.attribute.err_lex, "RUN TIME ERROR: ");
			currentToken.code = RTE_T;
			return currentToken;

		default: //else break and go to the other cases below
			break;
		} // end switch


		/* Part 2: Implementation of Finite State Machine (DFA)
			   or Transition Table driven Scanner
			   Note: Part 2 must follow Part 1 to catch the illegal symbols
		*/
		
		state = getNextState(state, c);//get the next state
		lexStart = (short)bufferGetCPosition(sourceBuffer)-1; //set lexStart to initial position
		//-1 is due to errors with my buffer.c code
	
		//if an error occurs with bufferGetCPosition when assigning lexStart
		if (lexStart == RT_FAIL_1) { 
			errorNumber = RT_FAIL_2; 
			strcpy(currentToken.attribute.err_lex, "RUN TIME ERROR: ");
			currentToken.code = RTE_T; //display error message and return runtime error token
			return currentToken;
		}

		bufferSetMarkPosition(sourceBuffer, lexStart); //mark the start position
		
		while (stateType[state] == NOAS) { //loop until acceptable state is found
			c = bufferGetChar(sourceBuffer); //get next char
			state = getNextState(state, c); //find next state for that char based on current state and char
		} 

		if (stateType[state] == ASWR) { //if it is retractable, retract it
			bufferRetract(sourceBuffer);
		}
		
		lexEnd = (short)bufferGetCPosition(sourceBuffer); //set the correct end postion 
		lexLength = (int)lexEnd - (int)lexStart + 1; //calculate the lexLength
		//TODO check lexLenght is valid (shrtmin shrtmax)
		
		lexemeBuffer = bufferCreate((short)lexLength, DEFAULT_INC_FACTOR, 'f'); //create a temporary buffer of the correct size

		//check for error in the creation of that buffer
		if (lexemeBuffer == NULL) {
			errorNumber = RT_FAIL_2; 
			strcpy(currentToken.attribute.err_lex, "RUN TIME ERROR: ");
			currentToken.code = RTE_T; //display error message and return runtime error token
			return currentToken;
		}

		bufferReset(sourceBuffer); //reset c pos back to the mark c position

		for (i = 0; i < lexLength-1; i++) 	//copy char in source buffer to lexeme buffer
		{ 
			c = bufferGetChar(sourceBuffer);
			if (bufferAddChar(lexemeBuffer, c) == NULL) { //if an error occurs while adding a char 
				errorNumber = RT_FAIL_2; 
				strcpy(currentToken.attribute.err_lex, "RUN TIME ERROR: ");
				currentToken.code = RTE_T; //display error message and return runtime error token
				return currentToken;
			}
		}
		
		bufferAddChar(lexemeBuffer, CHARSEOF0); //add the endchar to the lexemeBuffer to complete it
		currentToken = finalStateTable[state](lexemeBuffer->string); //call the correct function to parse the token 

		bufferFree(lexemeBuffer); //free the buffer 
		return currentToken; //return the token from part 2 
	} //end while loop
} //processToken

/*
* Function Name: getNextState
* Purpose: Goes through the tansition table and returns the correct state as an int
* Author: William Johns
* Version: 1.0 November 21 2020
* Called Functions: nextTokenClass()
* Parameters: int state - the current state
*			  char c	- if in debug mode, displays the current char as well as the state, column and next state
* Return Value: int equal to the next state 
* Algorithm: uses nextTokenClass to find the column value on the transition table that leads to the next state
*/
int getNextState(int state, char c) {
	int col;
	int next;
	if (debugMode)
		fprintf(stderr, "[03] getNextState\n");
	col = nextTokenClass(c);
	next = transitionTable[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	/*
	The assert(int test) macro can be used to add run-time diagnostic to programs
	and to "defend" from producing unexpected results.
	assert() is a macro that expands to an if statement;
	if test evaluates to false (zero) , assert aborts the program
	(by calling abort()) and sends the following message on stderr:

	Assertion failed: test, file filename, line linenum

	The filename and linenum listed in the message are the source file name
	and line number where the assert macro appears.
	If you place the #define NDEBUG directive ("no debugging")
	in the source code before the #include <assert.h> directive,
	the effect is to comment out the assert statement.
	*/
	assert(next != IS);

	/*
	The other way to include diagnostics in a program is to use
	conditional preprocessing as shown bellow. It allows the programmer
	to send more details describing the run-time problem.
	Once the program is tested thoroughly #define DEBUG is commented out
	or #undef DEBUF is used - see the top of the file.
	*/
#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	return next;
}

/*
* Function Name: nextTokenClass
* Purpose: Checks the current char and returns the next column number in the transition table for the next state
* Author: William Johns
* Version: 1.0 November 21 2020
* Called Functions: nextTokenClass(), fprinf(), isdigit(), isalpha()
* Parameters: char c - current char 
* Return Value: int equal to the next state
* Algorithm:
*/
int nextTokenClass(char c) {
	int val = -1;
	if (debugMode)
		fprintf(stderr, "[04] NextTokenClass\n");

	/*
	THIS FUNCTION RETURNS THE COLUMN NUMBER IN THE TRANSITION
	TABLE st_table FOR THE INPUT CHARACTER c.
	SOME COLUMNS MAY REPRESENT A CHARACTER CLASS .
	FOR EXAMPLE IF COLUMN 2 REPRESENTS [A-Za-z]
	THE FUNCTION RETURNS 2 EVERY TIME c IS ONE
	OF THE LETTERS A,B,...,Z,a,b...z.
	PAY ATTENTION THAT THE FIRST COLOMN IN THE TT IS 0 (has index 0)
	*/

	/* TODO_05: the logic to return the next column in TT */
	/*				[A-z](0),	0(1),	[1-9](2),	.(3),	$(4),	"(5),	SEOF(6),	other(7) */
	if (isalpha(c)) {
		val = 0; //[A - z] (0)
	}
	else if (c == '0') {
		val = 1; //0(1)
	} 
	else if (c != '0' && isdigit(c)) {
		val = 2; //[1-9](2)
	}
	else if (c == '.') {
		val = 3; //.(3)
	}
	else if (c == '$') {
		val = 4; //$(4)
	}
	else if (c == '"') {
		val = 5; //"(5)
	}
	else if (c == CHARSEOF0 || c == CHARSEOF255) {
		val = 6; //SEOF(6)
	}
	else {
		val = 7; //other(7)
	}

	return val;
}

/*
* Function Name: aStateFuncAVID
* Purpose: Checks to see if the current lexeme is a valid AVID token or keyword token and returns the corresponding token with the correct values
* Author: William Johns
* Version: 1.0 November 21 2020
* Called Functions: isKeyword(), strlen(), strcpy()
* Parameters: char lexeme - character array to check to see if it is a valid AVID string
* Return Value: currentToken as either a keyword (if it is one), or as an AVID either of which has the correct values
* Algorithm:
*/
Token aStateFuncAVID(char lexeme[]) {
	Token currentToken = { 0 }; // do not change
	unsigned int i;

	if (isKeyword(lexeme) != RT_FAIL_2) //check if keyword
	{
		currentToken.code = KW_T; //set keyword values and return the keyword token
		currentToken.attribute.kwt_idx = isKeyword(lexeme);
		return currentToken; //return the correct token
	}
	//implied else (not a keyword)
	currentToken.code = AVID_T; //set the correct values

	if (strlen(lexeme) > VID_LEN) { //if the lexeme is too long end it at VID_LEN chars
		for (i = 0; i < VID_LEN; i++) {
			currentToken.attribute.vid_lex[i] = lexeme[i];  //set the correct values

		}
		currentToken.attribute.vid_lex[VID_LEN] = CHARSEOF0; //with CHARSEOF0 at the end
		return currentToken; //return the correct token
	}
	
	strcpy(currentToken.attribute.vid_lex, lexeme); //set the correct values cont'd
	return currentToken; //return the correct token
}

/*
* Function Name: aStateFuncSVID
* Purpose: Checks to see if the current lexeme is a valid SVID token (not too long) and returns a token with the correct values
* Author: William Johns
* Version: 1.0 November 21 2020
* Called Functions: strlen(), strcpy()
* Parameters: char lexeme - character array to check to see if it is a valid SVID string
* Return Value: currentToken a SVID token with the correct values
* Algorithm:
*/
Token aStateFuncSVID(char lexeme[]) {
	Token currentToken = { 0 };
	unsigned int i;

	currentToken.code = SVID_T; //set the correct code

	//if too long
	if (strlen(lexeme) > VID_LEN) {
		for (i = 0; i < (VID_LEN - 1); i++) {
			currentToken.attribute.vid_lex[i] = lexeme[i];
		}
		currentToken.attribute.vid_lex[VID_LEN - 1] = '$'; 
		currentToken.attribute.vid_lex[VID_LEN] = CHARSEOF0; //set the correct attributes

		return currentToken; //return the token
	}

	strcpy(currentToken.attribute.vid_lex, lexeme); //set the correct attributes

	return currentToken; //return the token 
}

/*
* Function Name: aStateFuncIL
* Purpose: Checks to see if the current lexeme is a valid IL token (not too long or too large/small a number) 
*		   and returns a token with the correct values
* Author: William Johns
* Version: 1.0 November 21 2020
* Called Functions: atol(), strlen(), strcat(), strcpy()
* Parameters: char lexeme - character array to check to see if it is a valid IL 
* Return Value: currentToken a IL token with the correct values
* Algorithm:
*/
Token aStateFuncIL(char lexeme[]) {
	Token currentToken = { 0 };
	unsigned int i;
	long il; //temporary long (since it needs to be larger than a short to check if it is within range

	il = atol(lexeme);  //convert lexeme to long

	if (il > SHRT_MAX || il < SHRT_MIN) //if too big or too small
	{
		currentToken.code = ERR_T;  // set it as an error token 
		if (strlen(lexeme) > ERR_LEN) { //if it contains too many char shorten it
			for (i = 0; i < (ERR_LEN - 3); i++) {
				currentToken.attribute.err_lex[i] = lexeme[i];
			}
			strcat(currentToken.attribute.err_lex, "...");
		} 
		else //else not longer than the error len
		{
			strcpy(currentToken.attribute.err_lex, lexeme);
		}
		return currentToken; //return the error token
	}
	//implied else, it is a valid IL token, set its values
	currentToken.code = INL_T;
	currentToken.attribute.int_value = (short)il;

	return currentToken; //return the valid IL token
}

/*
* Function Name: aStateFuncFPL
* Purpose: Checks to see if the current lexeme is a valid FPL token (not too long or too large/small a number)
*		   and returns a token with the correct values
* Author: William Johns
* Version: 1.0 November 21 2020
* Called Functions: atof(), strlen(), strcat(), strcpy()
* Parameters: char lexeme - character array to check to see if it is a valid FPL
* Return Value: currentToken a FPL token with the correct values
* Algorithm:
*/
Token aStateFuncFPL(char lexeme[]) {
	Token currentToken = { 0 };
	unsigned int i;
	double fpl;

	fpl = atof(lexeme); //convert it into a double, must be larger than a float to check if it is within range

	if (fpl > FLT_MAX || (fpl < FLT_MIN && fpl != 0)) { //not within range
		currentToken.code = ERR_T; //set it as an error token
		if (strlen(lexeme) > ERR_LEN) {
			for (i = 0; i < (ERR_LEN - 3); i++) {	//if the error string would be long 
				currentToken.attribute.err_lex[i] = lexeme[i]; 
			}
			strcat(currentToken.attribute.err_lex, "...");
		}
		else //if the errored lexeme is shorter
		{
			strcpy(currentToken.attribute.err_lex, lexeme);
		}
		return currentToken; //return the error token

	} //implied else, valid FPL 

	currentToken.code = FPL_T; //set it as a FPL token
	currentToken.attribute.flt_value = (float)fpl; //convert the double to a float and store it

	return currentToken; //return valid FPL token
}

/*
* Function Name: aStateFuncSL
* Purpose: Checks to see if the current lexeme is a valid SL token (doesn't contain the '"' surrounding it)
*		   and returns a token with the correct values
* Author: William Johns
* Version: 1.0 November 21 2020
* Called Functions: bufferGetAddCPosition(), bufferAddChar()
* Parameters: char lexeme - character array to check to see if it is a valid SL
* Return Value: currentToken a SL token with the correct values
* Algorithm:
*/
Token aStateFuncSL(char lexeme[]) {
	Token currentToken = { 0 };
	unsigned int i;

	//get the correct string offset for the stringLiteralTable so that it is stored at the correct place
	currentToken.attribute.str_offset = bufferGetAddCPosition(stringLiteralTable)-1;

	for (i = 0; i < strlen(lexeme); i++) { //copy it into the stringLiteralTable
		if (lexeme[i] != '"') {//don't include the " that surround it
			bufferAddChar (stringLiteralTable, lexeme[i]);
		} 

		if ( lexeme[i] == '\n') { //if it is a newline char increment the line counter
			line++;
		}
	}
	
	bufferAddChar(stringLiteralTable, CHARSEOF0); //add \0 to the end
	currentToken.code = STR_T; //set the correct values
	return currentToken; //return the correct SL token
}

/*
* Function Name: aStateFuncErr
* Purpose: Checks to see if the current lexeme is a valid Error token (lexeme isn't too large)
*		   and returns a token with the correct values
* Author: William Johns
* Version: 1.0 November 21 2020
* Called Functions: strlen(), strcat(), strcpy()
* Parameters: char lexeme - character array to convert into a valid Error token
* Return Value: currentToken a Error token with the correct values
* Algorithm:
*/
Token aStateFuncErr(char lexeme[]) {
	Token currentToken = { 0 };
	unsigned int i; 

	currentToken.code = ERR_T; //set as an error token

	if (strlen(lexeme) > ERR_LEN) { //if the error token is too long 
		for (i = 0; i < ERR_LEN - 3; i++) {
			currentToken.attribute.err_lex[i] = lexeme[i];
		}
		strcat(currentToken.attribute.err_lex, "..."); //end and then add the ... to it 
	}
	else //if shorter or equal 
	{ 
		strcpy(currentToken.attribute.err_lex, lexeme); //get the correct values
	}

	//check for next line char
	for (i = 0; i < strlen(lexeme); i++) {
		if (lexeme[i] == '\n') {
			line++; //if there is a line terminator increment the line counter
		}
	}
	return currentToken; //return the valid error token
}

/*
* Function Name: isKeyword
* Purpose: Checks the keyword table and compares those values to the kw_lexeme in its parameters
* Author: William Johns
* Version: 1.0 November 21 2020
* Called Functions: strcmp()
* Parameters: char kw_lexeme - character array to check to see if its in the keyword table
* Return Value: int -2 if the kw_lexeme isn't in the keyword table
*				int equal to its place in the keyword table if it matched the kw_lexeme
* Algorithm:
*/
int isKeyword(char* kw_lexeme) {
	int i;
	
	for (i = 0; i < KWT_SIZE; i++) { //go through the keyword table
		if (strcmp(kw_lexeme, keywordTable[i]) == 0) {
			return i; //returns the index if they match
		}
	}
	
	//return error if it can't find it 
	return RT_FAIL_2;
}
