/*
* File Name: table.h
* Compiler: MS Visual Studio 2019
* Author: William Johns, 040 832 509
* Course: CST8152 - Compilers, Lab Section: 013
* Assignment: 2 - Scanner (A2)
* Date: 21st Nov 2020
* Proffesor/Original file from: Paulo Sousa
* Purpose: Contains the declarations for all the methods and tables used in scanner.c, as well as the constants that accompagny those tables
* Function list: aStateFuncAVID(), aStateFuncSVID(), aStateFuncIL(), aStateFuncFPL(), aStateFuncSL(), aStateFuncErr()  
*/


/* Macro section */
#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef TOKEN_H_
#include "token.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

 /*  Source end-of-file (SEOF) sentinel symbol
  *    '\0' or one of 255,0xFF,EOF
  */
/* TODO_02: EOF definitions */
#define CHARSEOF0 '\0'
#define CHARSEOF255 255 

/*  Special case tokens processed separately one by one
 *  in the token-driven part of the scanner
 *  '=' , ' ' , '(' , ')' , '{' , '}' , == , <> , '>' , '<' , ';',
 *  white space
 *  !!comment , ',' , ';' , '-' , '+' , '*' , '/', ## ,
 *  _AND_, _OR_, _NOT_ , SEOF,
 */

/* TODO_03: Error states and illegal state */
//used in the table below
#define ES  11		/* Error state  with no retract */
#define ER  12		/* Error state  with retract */
#define IS  -1		/* Illegal state */

#define MAX_INT_P 32767

 /* State transition table definition */
#define TABLE_COLUMNS 8

/* Transition table - type of states defined in separate table */
/* TODO_04: Define Transition table missing values */
//made using the TT attached to this lab
int  transitionTable[][TABLE_COLUMNS] = {
	/*				[A-z](0),	0(1),	[1-9](2),	.(3),	$(4),	"(5),	SEOF(6),	other(7)	*/
	/*				L,			Z,		N,			P,		S,		Q,		E,			O			*/
	/* State 00 */  {1,			6,		4,			ES,		ES,		9,		ES,			ES},		
	/* State 01 */  {1,			1,		1,			2,		3,		2,		2,			2},			
	/* State 02 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS},		
	/* State 03 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS},		
	/* State 04 */  {ES,		4,		4,			7,		5,		ES,		5,			5},			
	/* State 05 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS},		
	/* State 06 */  {ES,		6,		ES,			7,		5,		ES,		5,			5},			
	/* State 07 */  {8,			7,		7,			8,		8,		8,		8,			8},			
	/* State 08 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS},		
	/* State 09 */  {9,			9,		9,			9,		9,		10,		ER,			9},			
	/* State 10 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS},		
	/* State 11 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS},		
	/* State 12 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS}			
};

/* Accepting state table definition */
/* TODO_05: Define accepting states types */
//values used to determine stateType
#define ASWR	1	/* accepting state with retract */
#define ASNR	2	/* accepting state with no retract */
#define NOAS	3	/* not accepting state */ 

/* List of acceptable states */
/* TODO_06: Define list of acceptable states */
int stateType[] = {
	NOAS, /* State 00 */
	NOAS, /* State 01 */
	ASWR, /* State 02 AVID */ 
	ASNR, /* State 03 SVID */
	NOAS, /* State 04 */
	ASWR, /* State 05 IL */
	NOAS, /* State 06 */
	NOAS, /* State 07 */
	ASWR, /* State 08 FPL */
	NOAS, /* State 09 */
	ASNR, /* State 10 SL */
	ASNR, /* State 11 ERR NO RETRACT */
	ASWR, /* State 12 ERR W/ RETRACT */
};

/* Accepting action function declarations */
/* TODO_07: Declare accepting states functions */
Token aStateFuncAVID(char lexeme[]); /* State 02 AVID */
Token aStateFuncSVID(char lexeme[]); /* State 03 SVID */
Token aStateFuncIL(char lexeme[]); /* State 05 IL*/
Token aStateFuncFPL(char lexeme[]); /* State 08 FPL */
Token aStateFuncSL(char lexeme[]); /* State 10 SL */
Token aStateFuncErr(char lexeme[]); /* State 11/12 ERROR */


/* Defining a new type: pointer to function (of one char * argument)
   returning Token
*/
typedef Token(*PTR_AAF)(char* lexeme);

/* Accepting function (action) callback table (array) definition */
/* If you do not want to use the typedef, the equvalent declaration is:
 * Token (*finalStateTable[])(char lexeme[]) = {
 */
/* TODO_08: Define final state table */
PTR_AAF finalStateTable[] = {
	/* State 00 */ NULL,
	/* State 01 */ NULL,
	/* State 02 */ aStateFuncAVID,
	/* State 03 */ aStateFuncSVID,
	/* State 04 */ NULL,
	/* State 05 */ aStateFuncIL,
	/* State 06 */ NULL,
	/* State 07 */ NULL,
	/* State 08 */ aStateFuncFPL,
	/* State 09 */ NULL,
	/* State 10 */ aStateFuncSL,
	/* State 11 */ aStateFuncErr,
	/* State 12 */ aStateFuncErr //uses same as state 12. since both are errors although only one is retractable
};

/* Keyword lookup table (_AND_, _OR_ and _NOT_ are not keywords) */
/* TODO_09: Define the number of Keywords from the language */
#define KWT_SIZE 10 //changed from unknown, 9 if only using the other file

/* Keyword list */
/* TODO_10: Define the list of keywords */
char* keywordTable[] = {
	"PROGRAM",
	"IF",
	"THEN",
	"ELSE",
	"WHILE",
	"DO",
	"INPUT",
	"OUTPUT",
	"TRUE",
	"FALSE"
};

#endif
