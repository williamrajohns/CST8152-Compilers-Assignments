/* 
* File Name: buffer.h
* Compiler: MS Visual Studio 2019
* Author: William Johns, 040 832 509
* Course: CST8152 - Compilers, Lab Section: 013
* Assignment: 1 - Buffer (A1)
* Date: 10th Oct 2020
* Proffesor/Original file from: Paulo Sousa 
* Purpose: Contains the declarations for all the methods in buffer.c and the constants used by both testBuffer.c and buffer.c 
* Function list: bufferCreate(), bufferAddChar(), bufferClear(), bufferFree(), bufferIsFull(), bufferGetAddCPosition(),
*				bufferGetCapacity(), bufferGetOpMode(), bufferGetIncrement(), bufferLoad(), bufferIsEmpty(),
*				bufferGetChar(), bufferPrint(), bufferSetEnd(), bufferRetract(), bufferReset(), bufferRewind(),
*				bufferGetString(), bufferSetMarkPosition(), bufferGetFlags()
*/

#ifndef BUFFER_H_
#define BUFFER_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */
/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

/* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

/* constant definitions */
#define RT_FAIL_1 (-1)         /* operation failure return value 1 */
#define RT_FAIL_2 (-2)         /* operation failure return value 2 */
#define LOAD_FAIL (-2)         /* load fail return value */

#define DEFAULT_INIT_CAPACITY 200   /* default initial buffer capacity */
#define DEFAULT_INC_FACTOR 15       /* default increment factor */


/* You should add your own constant definitions here */
/* definition of contants I created*/
#define	MAXIMUM_POSITIVE_VALUE (SHRT_MAX - 1) //Maximum positive value used to add more space to the buffer
#define INCREMENT_100 100 //used to divide the increment by 100 for OpMode 'm'
#define INCREMENT_ERROR_VAL 0x100 //used as the return in the event the increment causes an error

/* Add your bit-masks constant definitions here */
#define DEFAULT_FLAGS  0xFFFC 	// 1111.1111 1111.1100
#define SET_EOB	0x0001			// 0000 0000 0000 0001
#define RESET_EOB 0xFFFE		// 1111 1111 1111 1110
#define CHECK_EOB 0x0001		// 0000 0000 0000 0001
#define SET_R_FLAG 0x0002		// 0000 0000 0000 0010
#define RESET_R_FLAG 0xFFFD		// 1111 1111 1111 1101
#define CHECK_R_FLAG 0x0002		// 0000 0000 0000 0010

/* user data type declarations */
typedef struct BufferEntity {
	char* string;   /* pointer to the beginning of character array (character buffer) */
	short capacity;    /* current dynamic memory size (in bytes) allocated to character buffer */
	short addCPosition;  /* the offset (in chars) to the add-character location */
	short getCPosition;  /* the offset (in chars) to the get-character location */
	short markCPosition; /* the offset (in chars) to the mark location */
	char  increment; /* character array increment factor */
	char  opMode;       /* operational mode indicator*/
	unsigned short flags;     /* contains character array reallocation flag and end-of-buffer flag */
} Buffer, * pBuffer;

/*
IMPORTANT NOTE (2DO):
Place your function declarations here.
Do not include the function header comments here.
Place them in the buffer.c file
*/
pBuffer bufferCreate(short initCapacity, char incFactor, char opMode);
pBuffer	bufferAddChar(pBuffer const pBE, char symbol);
int bufferClear(Buffer* const pBE);
void bufferFree(Buffer* const pBE);
int bufferIsFull(Buffer* const pBE);
short bufferGetAddCPosition(Buffer* const pBE);
short bufferGetCapacity(Buffer* const pBE);
int bufferGetOpMode(Buffer* const pBE);
size_t bufferGetIncrement(Buffer* const pBE);
int bufferLoad(FILE* const fi, Buffer* const pBE);
int bufferIsEmpty(Buffer* const pBE);
char bufferGetChar(Buffer* const pBE);
int bufferPrint(Buffer* const pBE, char newLine);
Buffer* bufferSetEnd(Buffer* const pBE, char symbol);
short bufferRetract(Buffer* const pBE);
short bufferReset(Buffer* const pBE);
int bufferRewind(Buffer* const pBE);
char* bufferGetString(Buffer* const pBE, short charPosition);
pBuffer bufferSetMarkPosition(pBuffer const pBE, short mark);
unsigned short bufferGetFlags(pBuffer const pBE);

#endif
