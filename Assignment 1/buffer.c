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
pBuffer bufferCreate(short initCapacity, unsigned char incFactor, char opMode) {
	/* New buffer in memory */ 
	Buffer* buffer; 
	/* calloc a buffer structure  */
	buffer = calloc(1, sizeof(Buffer));
	if (!buffer) {
		return NULL; /* if buffer doesn't have a pointer return null */
	}
	/* set all the positions to 0 */
	buffer->addCPosition = 0;
	buffer->getCPosition = 0;
	buffer->markCPosition = 0;

	/* if initial capacity is between 0 and MAXIMUM_POSITIVE_VALUE */
	if (initCapacity >= 0 && initCapacity <= MAXIMUM_POSITIVE_VALUE) {

		if (initCapacity == 0) /* default inputs*/
		{ 
			/*copies the given initCapacity	value into the Buffer's capacity variable */
			/*character buffer is 200 chars */
			buffer->string = malloc(200 * sizeof(char)); /*it also assigns this to string from the buffer */
			initCapacity = 200;
			
			
			if (opMode == 'f') { 
				buffer->increment = 0;
				incFactor = 0; 
			}
			else if (opMode == 'a' || opMode == 'm')
			{
				buffer->increment = DEFAULT_INC_FACTOR;
				incFactor = DEFAULT_INC_FACTOR;
			}
		}
		else //assumes initCapacity is larger than 0 and smaller than max pos value, but isn't 0;
		{  /* create the buffer with initCapcity if initCapacity is not set to default of 0*/
			buffer->string = malloc(initCapacity);
		}
		
		/*opMode sets */ 
		/* if opMode set to f - buffer increments set to 0 */
		if (opMode == 'f') {
			buffer->increment = 0;
			buffer->opMode = 0;
		}
		/*if opMode & initCapacity are set to 0 - Opmode and buffer increments set to 0 */
		if (incFactor == 0 && initCapacity != 0) { //What is there to see above?
			buffer->increment = 0;
			buffer->opMode = 0;
		}
		/*if opMode set to a & incFactor set between 1 and 255 - opMode is set to 1, and buffer increments sest to incFactor */
		if (opMode == 'a' && (int)incFactor >= 1 && (int)incFactor <= 255) {
			buffer->increment = incFactor; /*if opMode is 1 or -1, increment is incFactor */
			buffer->opMode = 1;
		}
		/*if opMode set to m & incFactor set between 0 and 100 - opMode is set to -1, buffer increments set to incFactor */
		if (opMode == 'm' && (int)incFactor >= 1 && (int)incFactor <= 100) {
			buffer->increment = incFactor;
			buffer->opMode = -1;
		}
		//don't create if incFactor is above 100 as it would cause errors when the capacity increases
		if (opMode == 'm' && (int)incFactor > 100)
		{
			return NULL;
		}
		
		/*copies the given initCapacity	value into the Buffer's capacity variable */
		buffer->capacity = initCapacity; /*asuming it is larger/equal to  0; */
		
		/*sets FLAGS to default (FFFC) value  */
		buffer->flags |= DEFAULT_FLAGS;
		
		/*if successful return a pointer to the Buffer  */
		return buffer;
	}
	/*implied else (lower than 0, bigger than max positive value) return NULL */
	return NULL;
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
	/*reset flags in r_flag to 0 */
	pBE->flags &= RESET_R_FLAG;

	/*if buffer is operational and not full - symbol is stored in the character buffer  */
	if (bufferIsFull(pBE)==0)
	{	
		pBE->string[pBE->addCPosition] = symbol; /* adds the symbol then increments for the next time it tries to add a symbol */
		pBE->addCPosition++;
		return pBE; /* returns the pointer */
	}
	else if (bufferIsFull(pBE) == 1) /* full */
	{
		if (pBE->opMode == 0) {
			return NULL; /*if opMode == 0, return NULL  since OpMode 0 is fixed in its capacity thus cannot be increased */
		}
		else if (pBE->opMode == 1) { /* additive opMode, increases by the increment */
			/*increase current capacity of buffer by adding increment to capacity */
			/*gets the correct new tempCapacity*/
			unsigned char tempchar = pBE->increment;
			long long tempCapacity = pBE->capacity + (int)tempchar;
			
			
			if (tempCapacity < MAXIMUM_POSITIVE_VALUE && tempCapacity > 0)
			{	/* else if tempcapacity is between 0 and the max positive value */
				pBE->capacity = (short) tempCapacity; 
			} else if (tempCapacity >= MAXIMUM_POSITIVE_VALUE || tempCapacity < 0)
			{ /*if adding to capacity would make it bigger than max pos value or underflow */
				pBE->capacity = MAXIMUM_POSITIVE_VALUE; /*become max value */
			}

			/*if adding would overflow it and turn it negative */
			if (pBE->capacity < 0)  
			{
				return NULL;
			}
		}
		else if (pBE->opMode == -1) { /* if opMode is in multiplicative */
			if (pBE->capacity >= MAXIMUM_POSITIVE_VALUE) {
				return NULL; /*if already max capacity return null */
			}
			/*calculation for new capacity*/
			int availableSpace = (int)MAXIMUM_POSITIVE_VALUE - (int)pBE->capacity;
			int newIncrement = (int)(availableSpace * (int)pBE->increment) / INCREMENT_100; 
			int newCapacity = (int)pBE->capacity + newIncrement;
			
			/*if newCapacity is smaller or eqaul to the current capacity but still smaller than the max pos value*/
			if (newCapacity <= (int)pBE->capacity && pBE->capacity <= MAXIMUM_POSITIVE_VALUE) {
				newCapacity = MAXIMUM_POSITIVE_VALUE;
			}
			/* if the new capacity is larger than the max pos value set it to the max pos value*/
			if (newCapacity >= MAXIMUM_POSITIVE_VALUE) {
				pBE->capacity = MAXIMUM_POSITIVE_VALUE;
			}
			else if (newCapacity < MAXIMUM_POSITIVE_VALUE) {
				pBE->capacity = (short)newCapacity;
			}
			else {
				return NULL; /*return null if it messes up */
			}

		}
		/*if modes 1 or -1 are successfull  (if mode 0 it has already returned)  */
			
		/*to compare address if it changes */
		int *ptr = realloc(pBE->string, pBE->capacity); /*makes the buffer the size of the new capacity */
		if (ptr != NULL)
		{ //if the memory location has been changed set reallocation flag
			pBE->string = ptr;
			pBE->flags |= SET_R_FLAG; /* if address has changed set the flag */
		}
		else {
			return NULL;
		}	
		
		/* if it hasn't reached capacity*/
		if (bufferIsFull(pBE) == 0)
		{ /*add the char*/
			pBE->string[pBE->addCPosition] = symbol;
			pBE->addCPosition++;
		}
		else if (bufferIsFull(pBE) == -1) {
			return NULL;
		}
		/* returns the pointer*/
		return pBE;
	}

	/*add checks for the type size of vars (16, 32, 64...) so no errors can occur */
	return NULL;
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
	free(pBE->string); /*free the char buffer */
	free(pBE); /*free the buffer pointer */
	return; /*this should not result in errors */
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
	} else if ((size_t)(pBE->addCPosition) == (size_t)pBE->capacity) 
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
	if (pBE->addCPosition >= 0) {
		return pBE->addCPosition;
	}
	/*if a run time error is possible return -1; */
	return RT_FAIL_1;
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
	if (pBE->capacity >= 0) {
		return pBE->capacity;
	}	/*if a run time error is possible return -1; */
	return RT_FAIL_1;
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
	if (pBE !=NULL) {
		int intOpMode = (int)pBE->opMode;
		return intOpMode;
	}
	/*if a run time error is possible return a value to where it was called; */
	return RT_FAIL_2; /* can't be -1 since that is a valid opMode */
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
	if (pBE != NULL) {
		/*returns non negative value of increment  */
		int intInc = (unsigned char)pBE->increment;
		if (intInc >= 0)
		{ /*must check that it is the right data type */
			size_t sizeTInc = (size_t)intInc;
			return sizeTInc;
		}
	}
	/*if a runt time error is possible return a value of  */
	return INCREMENT_ERROR_VAL; /*on error return 0x100 */
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
	
	if (fi != NULL && pBE != NULL) {
		int count = 0;
		char tempChar;
		while (feof(fi)==0) /* if end of file break */
		{
			tempChar = (char)fgetc(fi);
			
			if (bufferAddChar(pBE, tempChar) == NULL) {
				ungetc(tempChar, fi);
				return LOAD_FAIL; /*if couldn't load properly return -2 */
			}
			count++; /*increase count for every char added, not including the feof char */
		}
		pBE->addCPosition--;
		return count;
	}
	return RT_FAIL_1; /*if errors occur return -1 */
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
	if (pBE != NULL) {
		if (pBE->getCPosition == pBE->addCPosition) 
		{
			pBE->flags |= SET_EOB;
			return 0;
		}

		//reset eob flag
		pBE->flags = pBE->flags & RESET_EOB;
		char tempchar = (pBE->string[pBE->getCPosition]);
		pBE->getCPosition++; /*if successful increment c position, and return char located at that position */
		return tempchar; /*return value at new position */
		
	}
	/*possible run time error, return -2; */
	return RT_FAIL_2;
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
	char tempChar;
	while(1) {
		tempChar = bufferGetChar(pBE);
		if ((pBE->flags & CHECK_EOB) == 1)/*check end of buffer flag that bufferGetChar returns */
		{
			break;
		}
		/*display char from bufferGetChar */
		printf("%c", tempChar); 
		count++;
	}

	if (newLine != '0')
	{
		printf("\n"); /* if newLine is not equal to 0  then add a newline character */
	}

	if (count >= 0) {
		return count; /*does does not include the one from newLine */
	}
	return -1; /*otherwise return -1 on failure */
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
	if (pBE != NULL) {
		pBE->capacity = pBE->getCPosition + 1;
		Buffer* ptr = realloc(pBE->string, (size_t)pBE->capacity);/*it reallocs the capacity to (getCPosition+1) in bytes  */
		if (ptr != NULL)
		{ //if the memory location has been changed set reallocation flag
			pBE->string = ptr;
			pBE->flags |= SET_R_FLAG; /* if address has changed set the flag */
		}
		else 
		{ //if problem, reeturn null
			return NULL;
		}
		pBE->string[pBE->addCPosition] = symbol;/* add symbol to end of string */
		pBE->addCPosition++;/* then increment addCPosition; */
		
		/* THIS LINE EXISTS ONLY SO THAT THE FLAGS ARE THE SAME OUTPUT AS THE FILES FOR STUDENTS - IT IS UNNEEDED OTHERWISE */
		pBE->flags &= RESET_R_FLAG;

		return ptr;
	}	
	/*return NULL; /*if it cannot perform as needed */
	return NULL;
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
	if (pBE!= NULL) {
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
	if (pBE != NULL) { 
		pBE->markCPosition = 0; 
		pBE->getCPosition = 0;
		return 0; /*if successful return 0 */
	}
	/*if a run time error occurs,  */
	return RT_FAIL_1;
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
	if (pBE != NULL) {
		if ((int)strlen(pBE->string) < charPosition || charPosition < 0) 
		{ //if the stringlen is smaller than the position wanted, or th position is negative
			return NULL; /*if run time error is possible return NULL; */
		}
		return (&pBE->string[charPosition]);
	}
	printf("crashed from bufferGetString");
	return NULL;
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
	if (pBE != NULL) {
		return pBE->flags; /*return flag field */
	}
	/*since the return type is unsigned would this return the max value? */
	return (unsigned short)RT_FAIL_1; /* if a run time error is possible return -1  underflows to 0xFFFF*/
}

#endif