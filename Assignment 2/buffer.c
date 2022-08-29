/*
* File Name: buffer.c
* Compiler: MS Visual Studio 2019
* Author: William Johns, 040 832 509
* Course: CST8152 - Compilers, Lab Section: 013
* Assignment: 1 - Buffer (A1)
* Date: 10th Oct 2020
* Proffesor: Paulo Sousa
* Purpose: Contains the definitions for all the methods declared in buffer.h
*		   The methods are called from testBuffer.c to create a buffer, go through a file one character at a time and add them to the buffer.
*		   Then it displays the contents of the buffer, and the buffer parameters.
* Function list: bufferCreate(), bufferAddChar(), bufferClear(), bufferFree(), bufferIsFull(), bufferGetAddCPosition(),
*				bufferGetCapacity(), bufferGetOpMode(), bufferGetIncrement(), bufferLoad(), bufferIsEmpty(),
*				bufferGetChar(), bufferPrint(), bufferSetEnd(), bufferRetract(), bufferReset(), bufferRewind(),
*				bufferGetString(), bufferSetMarkPosition(), bufferGetFlags()
*/


#ifndef BUFFER_C_
#define BUFFER_C_

#include "buffer.h"
#include <string.h>


/*
* Function Name: bufferCreate
* Purpose: Creates a new buffer and if it has valid parameters sets the correct values before returning a pointer
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: short initCapacity, initial capacity used to create the buffer
				(range SHRT_MIN to SHRT_MAX, but won't create a buffer if below 0 or above MAX_POSITIVE_VALUE)
*		      unsigned char incFactor, char sent by testBuffer.c to determine the correct increment for the buffer (max 256)
*			  char opMode, the input from console which decides what mode the buffer will have (only valid ones are a, f, and m, others return error)
* Return Value: pointer to the created buffer if valid inputs, NULL if invalid
* Algorithm: Many if statements used to create buffer. If the buffer cannot be initialized, returns NULL
*/
pBuffer bufferCreate(short initCapacity, char incFactor, char opMode) {
	/* New buffer in memory */
	pBuffer buffer;
	char bMode;
	if (initCapacity < 0 || initCapacity > MAXIMUM_POSITIVE_VALUE) {
		return NULL;
	}
	if (!initCapacity) {
		initCapacity = DEFAULT_INIT_CAPACITY;
		incFactor = DEFAULT_INC_FACTOR;
	}

	if (!incFactor) {
		opMode = 'f';
	}

	switch (opMode) {
	case 'f':
		bMode = 0;
		incFactor = 0;
		break;
	case 'a':
		bMode = 1;
		break;
	case 'm':
		bMode = -1;
		if ((unsigned char)incFactor > 100) {
			return NULL;
		}
		break;
	default:
		return NULL;
	}

	buffer = (pBuffer)calloc(1, sizeof(Buffer));
	if (!buffer) {
		return NULL;
	}

	buffer->string = (char*)malloc(initCapacity);
	if (!buffer->string) {
		free(buffer);
		return NULL;
	}
	buffer->opMode = bMode;
	buffer->capacity = initCapacity;
	buffer->increment = incFactor;
	buffer->flags = DEFAULT_FLAGS;
	return buffer;

}

/*
* Function Name: bufferAddChar
* Purpose: Adds chars to the buffer, if the new char is past the capacity, try to increase capacity.
		   If it cannot increase capacity return NULL.
		   If it can increase capacity check to see if the memory location of the string is changed and set R_Flag accordingly
* Author: William Johns
* Version: 1.0 October 10 2020
* Called Functions: bufferIsFull()
* Parameters: pBuffer const pBE, pointer to the buffer
*			  char symbol, symbol it is trying to add to the buffer
* Return Value: buffer pointer if it works
*				NULL, if it runs into errors
* Algorithm: Resets r flag, then checks if it the buffer is not full and operational, if it is it adds the symbol to the buffer and increments addCPosition
*			 If it is operational and full it tries to increase the capacity (assuming it is not in fixed more) by incrementing it.
*			 If it cannot increment, returns NULL
*			 If it can increments, then continues trying to add the symbol to the end of the buffer
*/
pBuffer	bufferAddChar(pBuffer const pBE, char symbol)
{
	char* tempString;
	short availSpace, newSize, newInc;

	if (!pBE) {
		return NULL;
	}
	pBE->flags &= RESET_R_FLAG;

	if (pBE->addCPosition * sizeof(char) == MAXIMUM_POSITIVE_VALUE ||
		pBE->addCPosition * sizeof(char) == SHRT_MAX) {
		return NULL;
	}

	if (pBE->addCPosition == pBE->capacity) {

		switch (pBE->opMode) {
		case 0:
			return NULL;
		case 1:
			newSize = pBE->capacity + (unsigned char)pBE->increment;
			if (newSize > 0) {
				if (newSize == SHRT_MAX) {
					newSize = MAXIMUM_POSITIVE_VALUE;
				}
			}
			break;
		case -1:
			availSpace = MAXIMUM_POSITIVE_VALUE - pBE->capacity;
			newInc = (short int)(availSpace * (pBE->increment / INCREMENT_100));
			newSize = pBE->capacity + newInc;
			if (newSize <= pBE->capacity && pBE->capacity < MAXIMUM_POSITIVE_VALUE) {
				newSize = MAXIMUM_POSITIVE_VALUE;
			}
			break;
		default:
			return NULL;
		}
		tempString = (char*)realloc(pBE->string, newSize);
		if (!tempString) {
			return NULL;
		}
		if (tempString != pBE->string) {
			pBE->flags |= RESET_R_FLAG;
			pBE->string = tempString;
		}
		pBE->capacity = newSize;
	}
	pBE->string[pBE->addCPosition++] = symbol;
	return pBE;
}

/*
* Function Name: bufferClear
* Purpose: sets the positions to 0 and resets the flags
* Author: William Johns
* Version: 1.0 October 10 2020
* Called Functions:
* Parameters: Buffer const pBE, pointer to the buffer
* Return Value: returns 0 if successful and -1 if an error happens
* Algorithm:
*/
int bufferClear(Buffer* const pBE) {
	/*returns a "clear" buffer by reinitializing the values */
	if (pBE != NULL) {
		pBE->addCPosition = 0;
		pBE->getCPosition = 0;
		pBE->markCPosition = 0;
		pBE->flags &= RESET_EOB;
		pBE->flags &= RESET_R_FLAG;
		return 0;
	}
	/* if anything goes wrong  */
	return RT_FAIL_1;
}

/*
* Function Name: bufferFree
* Purpose: frees the string and the buffer pointer
* Author: William Johns
* Version: 2.0 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer it is going to free
* Return Value:
* Algorithm:
*/
void bufferFree(Buffer* const pBE) {
	if (pBE) {
		free(pBE->string); /*free the char buffer */
		free(pBE); /*free the buffer pointer */
	}
}

/*
* Function Name: bufferIsFull
* Purpose: Returns 1 if the buffer is full, 0 if it is not full
* Author: William Johns
* Version: 2.0 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the pointer to the buffer
* Return Value: Returns an int of 1 if the buffer is full, 0 if it is not full, -1 if an error has occured
* Algorithm:
*/
int bufferIsFull(Buffer* const pBE) {
	if (pBE == NULL)
	{/*if a run time error is possible return -1; */
		return RT_FAIL_1;
	}
	else if ((size_t)(pBE->addCPosition) == (size_t)pBE->capacity)
	{ /*could be changed to call bufferGetCapacity */
		return 1; /*returns 1 if the buffer is full */
	}
	/*returns 0 if not full */
	return 0;
}

/*
* Function Name: bufferGetAddCPosition
* Purpose: returns addCPosition
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
* Return Value: returns addCPosition as a short if it works, -1 if an error occurs
* Algorithm:
*/
short bufferGetAddCPosition(Buffer* const pBE) {
	if (!pBE) {
		return RT_FAIL_1;
	}
	return pBE->addCPosition+1;
}

/*
* Function Name: bufferGetCapacity
* Purpose: returns the capacity of the buffer
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
* Return Value: returns the capacity as a short if it works, -1 if an error occurs
* Algorithm:
*/
short bufferGetCapacity(Buffer* const pBE) {
	if (!pBE) {
		return RT_FAIL_1;
	}
	return pBE->capacity;
}

/*
* Function Name: bufferGetOpMode
* Purpose: returns the buffer's opMode
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
* Return Value:returns the buffer's opMode as a int if there isn't an error, -2 if an error occurs (can't be -1 since that is a valid opMode)
* Algorithm:
*/
int bufferGetOpMode(Buffer* const pBE) {
	if (!pBE) {
		return RT_FAIL_2;
	}
	return pBE->opMode;
}

/*
* Function Name: bufferGetIncrement
* Purpose: returns the buffers' increment
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters:
* Return Value: returns the buffer's increment in size_t if it works, if an error occurs it returns a special return value (0x100)
* Algorithm:
*/
size_t bufferGetIncrement(Buffer* const pBE) {
	if (!pBE) {
		return INCREMENT_ERROR_VAL;
	}
	return (unsigned char)pBE->increment;
}

/*
* Function Name: bufferLoad
* Purpose: loads chars into the buffer string one char at a time, stops when it reaches the end of the file or bufferAddChar returns NULL
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions: bufferAddChar
* Parameters: File* const fi, the file it is trying to parse through and copy into the buffer
*			  Buffer* const pBE, the buffer pointer
* Return Value: returns an int equal to the amount of chars added, if an error occurs it returns -1
* Algorithm:
*/
int bufferLoad(FILE* const fi, Buffer* const pBE) {
	int size = 0;
	char c;
	if (!fi || !pBE) {
		return RT_FAIL_1;
	}

	c = (char)fgetc(fi);
	while (!feof(fi)) {
		if (!bufferAddChar(pBE, c)) {
			ungetc(c, fi);
			return LOAD_FAIL;
		}
		c = (char)fgetc(fi);
		size++;
	}
	if (ferror(fi)) {
		return RT_FAIL_1;
	}
	return size;
}


/*
* Function Name: bufferIsEmpty
* Purpose: returns 1 if the buffer is empty, 0 if the buffer isn't empty. Does so by checking if addCPosition is at its starting value
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
* Return Value: an int of 1 if empty, 0 if not empty, -1 if an error occurs
* Algorithm:
*/
int bufferIsEmpty(Buffer* const pBE) {
	if (pBE != NULL)
	{
		if (pBE->addCPosition == 0)
		{ /*if it is empty  return 1 */
			return 1;
		}
		else
		{ /* otherwise return 0 */
			return 0;
		}
	}
	return RT_FAIL_1;
}

/*
* Function Name: bufferGetChar
* Purpose: returns a char from the buffer at the position of getCPosition then increments getCPosition. Used to display the buffer
		   checks the End of Buffer flag to see if it has reached its end
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
* Return Value: the char at getCposition at the time it is called, -2 if an error occurs
* Algorithm:
*/
char bufferGetChar(Buffer* const pBE) {
	/*reads the buffer  */
	if (!pBE) {
		return RT_FAIL_1;
	}

	if (pBE->getCPosition == pBE->addCPosition)
	{
		pBE->flags |= SET_EOB;
		return '\0';
	}
	pBE->flags &= RESET_EOB;
	char tempchar = pBE->string[pBE->getCPosition];
	pBE->string[pBE->getCPosition++];
	return tempchar;
}

/*
* Function Name: bufferPrint
* Purpose: loops through the buffer and prints out one char at a time the contents of the buffer string.
*		   if the newLine parameter is not 0, adds a newLine to the end
*		   then returns the count of how many chars it has displayed
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions: bufferGetChar
* Parameters: Buffer* const pBE, the buffer pointer
*			  char newLine, used to decide if it needs to add a new line to the end of the display of the buffer
* Return Value: an int equal to how many chars were in the buffer string, or if an error occurs -1
* Algorithm:
*/
int bufferPrint(Buffer* const pBE, char newLine) {
	int count = 0;
	char c;
	if (!pBE || !pBE->string) {
		return RT_FAIL_1;
	}

	c = bufferGetChar(pBE);
	while (!(pBE->flags & CHECK_EOB)) {
		printf("%c", c);
		count++;
		c = bufferGetChar(pBE);
	}
	if (newLine)
	{
		printf("\n");
	}
	return count;
}

/*
* Function Name: bufferSetEnd
* Purpose: sets the end of the buffer to a symbol given from another function then increments addCposition
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
*			  char symbol, the symbol to be put at the end of the buffer string
* Return Value: returns a buffer pointer if it is valid, returns NULL if an error occurs
* Algorithm:
*/
Buffer* bufferSetEnd(Buffer* const pBE, char symbol) {
	char* tempString;
	short size;
	if (!pBE || pBE->addCPosition * sizeof(char) == SHRT_MAX) {
		return NULL;
	}
	pBE->flags &= RESET_R_FLAG;
	size = (pBE->addCPosition + 1) * sizeof(char);
	tempString = (char*)realloc(pBE->string, size);
	if (!tempString) {
		return NULL;
	}
	if (pBE->string != tempString) {
		pBE->flags |= SET_R_FLAG;
		pBE->string = tempString;
	}
	pBE->capacity = size;
	pBE->string[pBE->addCPosition] = symbol;
	return pBE;
}

/*
* Function Name: bufferRetract
* Purpose: if it can, decrements getCPostion by 1
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
* Return Value: returns the new getCPosition as a short if valid, -1 if an error occurs or it is already at 0
* Algorithm:
*/
short bufferRetract(Buffer* const pBE) {
	if (pBE != NULL) {
		if ((pBE->getCPosition - 1) >= 0) {
			pBE->getCPosition--; /*decrements getCPosition */
			return pBE->getCPosition; /*return the position */
		}
	}
	return RT_FAIL_1; /*if there are problems return -1 */
}

/*
* Function Name: bufferReset
* Purpose: resets the getCPosition to markCPosition
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
* Return Value: returns the new getCPosition as a short if it is valid, returns -1 if an error occurs
* Algorithm:
*/
short bufferReset(Buffer* const pBE) {
	if (pBE != NULL) {
		if (pBE->markCPosition >= 0 && pBE->markCPosition <= pBE->capacity)
		{	/*maybe check to see if markCPosition is bigger than the capacity */
			pBE->getCPosition = pBE->markCPosition; /*set C position to mark position */
			return pBE->getCPosition; /*return the value */
		}
	}
	return RT_FAIL_1; /*if there are problems return -1 */
}

/*
* Function Name: bufferGetCPosition
* Purpose: returns a short equal to the getCPosition in the buffer
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
* Return Value: returns a short equal to getCPosition, returns -1 if an error occurs
* Algorithm:
*/
short bufferGetCPosition(Buffer* const pBE) {
	if (pBE != NULL) {
		if (pBE->getCPosition >= 0 && pBE->getCPosition <= pBE->capacity) {
			return pBE->getCPosition; /*returns getCposition */
		}
	}
	return RT_FAIL_1; /*if there are problems return -1 */
}

/*
* Function Name: bufferRewind
* Purpose: resets markCPosition and getCPosition to 0
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
* Return Value: returns an int of 0 if successful, -1 if an error occurs or it can't
* Algorithm:
*/
int bufferRewind(Buffer* const pBE) {
	if (!pBE) {
		return RT_FAIL_1;
	}
	pBE->getCPosition = 0;
	pBE->markCPosition = 0;
	return 0;
}

/*
* Function Name: bufferGetString
* Purpose: finds the character at a given position in the buffer, if an error occurs returns NULL
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
*			  short charPosition, what position in the string it should look for, if out of the range of the buffer (negative or larger than the buffer) returns 0
* Return Value: returns the char at the charPosition in the buffer, if charPosition isn't valid, or an error occurs returns NULL
* Algorithm:
*/
char* bufferGetString(Buffer* const pBE, short charPosition) {
	if (!pBE || charPosition < 0 || charPosition > pBE->addCPosition) {
		return NULL;
	}

	return pBE->string + charPosition;
}

/*
* Function Name: bufferSetMarkPosition
* Purpose: sets markCPosition to a short given from the parameter
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
*			  short mark, the new short markCPosition should be set to
* Return Value: returns a pointer to the buffer if successful, returns NULL otherwise
* Algorithm:
*/
pBuffer bufferSetMarkPosition(pBuffer const pBE, short mark) {
	if (mark > pBE->capacity || mark < 0)
	{
		return NULL; /*if it isn't within buffer return */
	}
	pBE->markCPosition = mark;
	return pBE; /*if successful return the new marked position */
}

/*
* Function Name: bufferGetFlags
* Purpose: returns the flags of the buffer
* Author: William Johns
* Version: 2.2 October 10 2020
* Called Functions:
* Parameters: Buffer* const pBE, the buffer pointer
* Return Value: returns a unsigned short equal to which flags are set if successful, returns -1 as an unsigned short if an error occurs
* Algorithm:
*/
unsigned short bufferGetFlags(pBuffer const pBE) {
	if (!pBE) {
		return (unsigned short)RT_FAIL_1; /*return flag field */
	}
	return pBE->markCPosition;
}

#endif