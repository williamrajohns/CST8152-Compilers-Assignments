/*
* File Name: parser.H
* Compiler: MS Visual Studio 2019
* Author: William Johns, 040 832 509
* Course: CST8152 - Compilers, Lab Section: 013
* Assignment: 3 - Parser (A3)
* Date: 12th Dec 2020
* Proffesor/Original file from: Paulo Sousa
* Purpose: Contains the declaration for the functions used parser.c,
*			Also includes global variable used in parser.c
*			Also defines the constants used for the keywords in parser.c
* Function list: program(), opt_statements(), statements(), statement(), statementsPrime(), assignment_statement(), assignment_expression(),
*					selection_statement(), iteration_statement(), pre_condition(), input_statement(), variable_list(), variable_identifier(),
*					variable_listPrime(), output_statement(), output_list(), opt_variable_list(), arithmetic_expression(), unary_arithmetic_expression(),
*					additive_arithmetic_expression(), additive_arithmetic_expressionPrime(), multiplicative_arithmetic_expression(), multiplicative_arithmetic_expressionPrime(),
*					primary_arithmetic_expression(), string_expression(), primary_string_expression(), string_expressionPrime(), conditional_expression(),
*					logical_or_expression(), logical_or_expressionPrime(), logical_and_expression(), logical_and_expressionPrime(), logical_not_expression(),
*					void relational_expression(), operator_list(), primary_a_relational_expression(), primary_s_relational_expression()
*/


/* Inclusion section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "token.h"
#include "buffer.h"

/* Global vars */
static Token lookahead; // define: lookahead
int syntaxErrorNumber; // define: syntaxErrorNumber
int debugMode = 0; // global variable to turn on/off debug mode, currently set to 0 so that the displays will be correct


extern Buffer* stringLiteralTable;
extern int line;
extern Token processToken();
extern char* keywordTable[];

/* Constants */
#define	NO_ATTR (-1)
// Keyword Constants matches order of keywordTable in table.h (0-9)
#define	PROGRAM 0
#define	IF 1
#define	THEN 2
#define	ELSE 3
#define	WHILE 4
#define	DO 5
#define	INPUT 6
#define	OUTPUT 7
#define	TRUE 8
#define	FALSE 9



/* Function definitions */
void processParser(void);
void matchToken(int, int);
void syncErrorHandler(int);
void printError();

// All FunctionsForNonTerminals
void program(void); //from parser_uncomplete.h
void opt_statements(void); //changed name to match my notes
void statements(void);
void statement(void);
void statementsPrime(void);
void assignment_statement(void);
void assignment_expression(void);
void selection_statement(void);
void iteration_statement(void);
void pre_condition(void);
void input_statement(void);
void variable_list(void);
void variable_identifier(void);
void variable_listPrime(void);
void output_statement(void);
void output_list(void);
void opt_variable_list(void);
void arithmetic_expression(void);
void unary_arithmetic_expression(void);
void additive_arithmetic_expression(void);
void additive_arithmetic_expressionPrime(void);
void multiplicative_arithmetic_expression(void);
void multiplicative_arithmetic_expressionPrime(void);
void primary_arithmetic_expression(void);
void string_expression(void);
void primary_string_expression(void);
void string_expressionPrime(void);
void conditional_expression(void);
void logical_or_expression(void);
void logical_or_expressionPrime(void);
void logical_and_expression(void);
void logical_and_expressionPrime(void);
void logical_not_expression(void);
void relational_expression(void);
void operator_list(void);
void primary_a_relational_expression(void);
void primary_s_relational_expression(void);