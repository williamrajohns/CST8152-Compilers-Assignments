/*
* File Name: parser.c
* Compiler: MS Visual Studio 2019
* Author: William Johns, 040 832 509
* Course: CST8152 - Compilers, Lab Section: 013
* Assignment: 3 - Parser (A3)
* Date: 12th Dec 2020
* Proffesor/Original file from: Paulo Sousa
* Purpose: Contains the definitions for the functions declared in parser.h, uses the grammar defined in PLATYPUS2_LGR_20F to parse the tokens
*			Contains many methods that are called to do this
*			Each one also prints out a message assuming it works
* Function list: program(), opt_statements(), statements(), statement(), statementsPrime(), assignment_statement(), assignment_expression(), 
*					selection_statement(), iteration_statement(), pre_condition(), input_statement(), variable_list(), variable_identifier(), 
*					variable_listPrime(), output_statement(), output_list(), opt_variable_list(), arithmetic_expression(), unary_arithmetic_expression(), 
*					additive_arithmetic_expression(), additive_arithmetic_expressionPrime(), multiplicative_arithmetic_expression(), multiplicative_arithmetic_expressionPrime(),
*					primary_arithmetic_expression(), string_expression(), primary_string_expression(), string_expressionPrime(), conditional_expression(),
*					logical_or_expression(), logical_or_expressionPrime(), logical_and_expression(), logical_and_expressionPrime(), logical_not_expression(),
*					void relational_expression(), operator_list(), primary_a_relational_expression(), primary_s_relational_expression()
*/

#include "parser.h"

/*
* Function Name: processParser
* Purpose: the 'main' function used to parse the tokens that have been assigned in the scanner 
*			calls the program() function which in term calls all the other functions needed to get through the file being parsed
* Author: William Johns
* Version: 1.0 December 12th 2020
* Called Functions: processToken(), program(), matchToken()
* Parameters: void
* Return Value: void
* Algorithm: if in debug mode prints a message in the console
*			sets lookahead to the next token and then calls program() which will try to parse that token
*			program() then goes through all the tokens until it reaches SEOF_T 
*			at which point processParser matches it and then prints a message showing it was successful
*/
void processParser(void) {
	if (debugMode)
		fprintf(stderr, "[00] Processing parser...\n");
	lookahead = processToken();
	program();
	matchToken(SEOF_T, NO_ATTR);
	printf("%s\n", "PLATY: Source file parsed");
}

/*
* Function Name: matchToken
* Purpose: tries to match a token's code (and attributes) with the code (and attributes) in its parameters
*			if unsuccessful returns after calling syncErrorHandler
* Author: William Johns
* Version: 1.0 December 12th 2020
* Called Functions: processToken(), printError(), syncErrorHandler()
* Parameters: int tokenCode - token code it is trying to match with
*			  int tokenAttribute - token attribute it will try to match with if the token is a kw/rel_op/art_op/log_op token
* Return Value: void
* Algorithm: sets the matchFlag to 0 if the tokenCode from the parameter doesn't match with the lookahead.code 
*			(if the lookahead.code token is a kw/rel_op/art_op/log_op token it also checks both attributes)
*			if it does match and the lookahead.code is SEOF_T it returns as it is the end of the tokens
*			if it does match otherwise it checks the next token and if its an error token handles it, and returns
*			if it doesn't match it calls syncErrorHandler() 
*/
void matchToken(int tokenCode, int tokenAttribute) {
	int matchFlag = 1; //1 for matches, 0 for doesn't match
	switch (lookahead.code) {
	case KW_T:
	case REL_OP_T:
	case ART_OP_T:
	case LOG_OP_T:
		if (lookahead.code != tokenCode || lookahead.attribute.get_int != tokenAttribute) { //if tokenCode or attrubute code of the lookahead token doesn't match parameters
				matchFlag = 0; //set to doesn't match 
		}
		break;
	default: //implied else (not a KW_T, REL_OP_T, ART_OP_T, LOG_OP_T)
		if (lookahead.code != tokenCode) { //if token code of the lookahead token doesn't match the tokenCode from parameters
			matchFlag = 0; //set to deosn't match
		}		
		break;
	}

	if (matchFlag && lookahead.code == SEOF_T) //if it matches and the next token is SEOF_T, return 
		return; 

	if (matchFlag) { //implied else, it matches but isn't an SEOF_T
		
		lookahead = processToken(); //advance to the next token

		
		if (lookahead.code == ERR_T) { //if new token is error
			printError(); //calls
			lookahead = processToken(); //advances to the next token
			syntaxErrorNumber++; //incremnets error number
			return; 
		}

		//there are directions in the notes that if the match isn't sucessful it should do something in this if statement
		//but this if statement is if match is successful
		//so I've moved it to TODO_09 below (in the else statement)
	}
	else //match is unsuccessful
	{ 
		syncErrorHandler(tokenCode); //if it doesn't match, call syncErrorHandler(tokenCode)
		return;
	}

	
}


/*
* Function Name: syncErrorHandler
* Purpose: acts as a basic error handler
* Author: William Johns
* Version: 1.0 December 12th 2020
* Called Functions: printError(), exit(), processToken()
* Parameters: int syncTokenCode - it is the token code it is trying to match with the lookahead token
* Return Value: void
* Algorithm: calls printError and increments the error number
*			then loops through the tokens until the lookahead token matches with the syncTokenCode
*			While doing this if the token is an SEOF_T it exits since it reached the end of the tokens
*			after exiting the loop if the token isn't a SEOF_T it lookahead goes to the next one and then returns
*/
void syncErrorHandler(int syncTokenCode) {
	
	printError(); //calls printError(), and then increments error number 
	syntaxErrorNumber++;

	while (lookahead.code != syncTokenCode) { //while the current token doens't match the syncTokenCode from the parameter
		
		//if looking for code different from SEOF_T and reaches the end it calls exit (syntaxErrorNumber)
		//since it gets the new lookahead token after this, if the syncTokenCode is same as SEOF_T it will exit the loop as they will match
		if (lookahead.code == SEOF_T) {
			exit(syntaxErrorNumber); //if it reaches the end of the source file it calls this
		}

		lookahead = processToken(); //advance to next token after ensureing that the end of file hasn't been reached
	}

	if (lookahead.code != SEOF_T) { //if the next token isn't a SEOF_T advance one token
		lookahead = processToken();	
	}
	return; //then return
		
}


/*
* Function Name: printError
* Purpose: Displays the corresponding error message
* Author: William Johns
* Version: 1.0 December 12th 2020
* Called Functions: printf()
* Parameters: 
* Return Value: void
* Algorithm: prints a general error message
*			then uses a switch case statement based on the lookahead token's code to handle what attribute to print alongside it
*/
void printError() {
	Token t = lookahead;
	if (debugMode)
		fprintf(stderr, "[39] PrintError\n");
	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code) { //all the cases are tokens 
	//In order based on token.h
	//with text to match the output files (as best I can)
	//Error
	case  ERR_T: // -1 - Error
		printf("%s\n", t.attribute.err_lex);
		break;
	//End Of File
	case  SEOF_T: // 0 - EOF 
		printf("SEOF_T\t\t%d\t\n", t.attribute.seof);
		break;
	
	//Variable Identifiers / Literals
	case  AVID_T: //  1 - AVID 
	case  SVID_T:// 2 - SVID 
		printf("%s\n", t.attribute.vid_lex);
		break;
	case  FPL_T: // 3 - FPL
		printf("%5.1f\n", t.attribute.flt_value);
		break;
	case INL_T: // 4 - IL 
		printf("%d\n", t.attribute.get_int);
		break;
	case STR_T:// 5 - SL 
		printf("%s\n", bufferGetString(stringLiteralTable, t.attribute.str_offset)); //print the string store in the stringLiteralTable at the offset store in the token4
		//ends when it reaches a \0 char iirc so it doesn't print the whole table
		break;

	//String Concat Token 
	case SCC_OP_T: // 6 - String Concatenation
		printf("NA\n");
		break;

	//Assignment / Arithmetic / Relational / Logical Operators
	case  ASS_OP_T:// 7 - Assignment Operator 
		printf("NA\n");
		break;
	case  ART_OP_T:// 8 - Arithmetic Operator 
		printf("%d\n", t.attribute.get_int);
		break;
	case  REL_OP_T: // 9 - Relational Operator
		printf("%d\n", t.attribute.get_int);
		break;
	case  LOG_OP_T:// 10 - Logical Operator 
		printf("%d\n", t.attribute.get_int);
		break;

	//Braces / Paranthsis Tokens
	case  LPR_T: // 11 - Left Parenthesis 
		printf("NA\n");
		break;
	case  RPR_T: // 12 - Right Parenthesis 
		printf("NA\n");
		break;
	case LBR_T: // 13 - Left Brace 
		printf("NA\n");
		break;
	case RBR_T: // 14 - Right Brace 
		printf("NA\n");
		break;

	//Keyword Tokens
	case KW_T: // 15 - Keyword 
		printf("%s\n", keywordTable[t.attribute.get_int]); //get the correct keyword using the keyword offset (get_int)
		break;

	//Comma and Semi-Colon Tokens
	case COM_T: // 16 - Comma 
		printf("NA\n");
		break;
	case EOS_T: // 17 - Semi-Colon (End Of Statment) 
		printf("NA\n");
		break;

	//Other 
	default: //from parser_uncomplete.c file - RTE_T (18) or other errors 
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}
}

/*************************************************************************************************************************
*			STARTING HERE THE FUNCTION HEADERS ARE JUST THEIR GRAMMAR NOTATIONS
* I'm unsure whether or not they need to be done according to the submission standard or what is described in the assignment specification
* Since they are all essentially the same I've chosen to do it this way
* They all (unless I copy pasted the wrong thing in) do what the grammar in the header says they do
* And the grammar in the header should match with the one in Platypus2_LGR_20F.doc
*************************************************************************************************************************/


/*************************************************************
 * Program statement
 * BNF: <program> -> PROGRAM { <opt_statements> }
 * FIRST(<program>) = {KW_T (PROGRAM)}.
 ************************************************************/
void program(void) { //one given in parser_uncomplete
	if (debugMode)
		fprintf(stderr, "[01] Program\n");
	matchToken(KW_T, PROGRAM);
	matchToken(LBR_T, NO_ATTR);
	opt_statements(); //optionalStatements() in parser_uncomplete.c - changed to match names in Platypus2_LGR_20F.doc
	matchToken(RBR_T, NO_ATTR);
	printf("%s\n", "PLATY: Program parsed");
}

/*************************************************************
 * opt_statements
 * BNF: <opt_statements> -> <statements> | e
 * FIRST(<opt_statements>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(INPUT),  KW_T(OUTPUT), e }
 ************************************************************/
void opt_statements(void) { //changed name to match Platypus2_LGR_20F.doc - original name was optionalStatements
	//otherwise taken from the assignment spec given
	switch (lookahead.code){
	case AVID_T: case SVID_T: //avid svid
		statements();
		break;
	case KW_T: //kw
		if (lookahead.attribute.get_int == IF || lookahead.attribute.get_int == WHILE ||
			lookahead.attribute.get_int == INPUT || lookahead.attribute.get_int == OUTPUT) 
		{
			statements();
			break;
		}
	default: //e
		printf("%s\n", "PLATY: Opt_statements parsed");
	}
}

/*************************************************************
 * statements
 * BNF: <statements> -> <statement><statementsPrime>
 * FIRST(<statements>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(INPUT), KW_T(OUTPUT) }
 ************************************************************/
void statements(void) {
	statement();
	statementsPrime();
}

/*************************************************************
 * statementsPrime
 * BNF: <statementsPrime> -> <statement><statementsPrime> | e
 * FIRST(<statementsPrime>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(INPUT), KW_T(OUTPUT), e }
 ************************************************************/
void statementsPrime(void) {
	switch (lookahead.code) { 
	case AVID_T: case SVID_T: //avid svid
		statement();
		statementsPrime();
		break;
	case KW_T: //kw
		if (lookahead.attribute.get_int == IF || lookahead.attribute.get_int == WHILE || 
			lookahead.attribute.get_int == INPUT || lookahead.attribute.get_int == OUTPUT) 
		{
			statement();
			statementsPrime();
		}
		break;
	} //else e
}

/*************************************************************
 * statement
 * BNF: <statement> ->  <assignment statement> | <selection statement> | <iteration statement> 
						| <input statement> | <output statement>
 * FIRST(<statement>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(INPUT), KW_T(OUTPUT) } 
 ************************************************************/
void statement(void) {
	switch (lookahead.code) {
	case AVID_T: case SVID_T:
		assignment_statement();
		break;
	case KW_T:
		if (lookahead.attribute.get_int == IF) { //coresponding function
			selection_statement();
		}
		else if (lookahead.attribute.get_int == WHILE) {
			iteration_statement();
		}
		else if (lookahead.attribute.get_int == INPUT) {
			input_statement();
		}
		else if (lookahead.attribute.get_int == OUTPUT) {
			output_statement();
		}
		break;
	default:
		printError();
		break;
	}
}

/*************************************************************
 * assignment_statement
 * BNF: <assignment_statement>  -> <assignment_expression>
 * FIRST(<assignment_statement>) = { AVID_T, SVID_T } 
 ************************************************************/
void assignment_statement(void) {
	assignment_expression();
	matchToken(EOS_T, NO_ATTR);
	printf("PLATY: Assignment statement parsed\n");
}

/*************************************************************
 * assignment_expression
 * BNF: <assignment_expression> -> AVID = <arithmetic expression> | SVID = <string expression>
 * FIRST(<assignment_expression>) = { AVID_T, SVID_T } 
 ************************************************************/
void assignment_expression(void) {
	switch (lookahead.code) {
	case AVID_T:
		matchToken(AVID_T, NO_ATTR); //avid 
		matchToken(ASS_OP_T, NO_ATTR); // = 
		arithmetic_expression(); //avid expression (number)
		printf("PLATY: Assignment expression (arithmetic) parsed\n");
		break;
	case SVID_T:
		matchToken(SVID_T, NO_ATTR); //svid
		matchToken(ASS_OP_T, NO_ATTR); // =
		string_expression(); //svid expression (string)
		printf("PLATY: Assignment expression (string) parsed\n");
		break;
	default: //something went wrong
		printError();
		break;
	}
}

/*************************************************************
 * selection_statement
 * BNF: <selection_statement> -> IF <pre_condition>  (<conditional_expression>)
 								 THEN { <opt_statements> }
   								 ELSE { <opt_statements> };
 * FIRST(<selection_statement>) = { KW_T(IF) } 
 ************************************************************/
void selection_statement(void) {
	matchToken(KW_T, IF); //if 
	pre_condition();
	matchToken(LPR_T, NO_ATTR); // ( 
	conditional_expression(); //condition
	matchToken(RPR_T, NO_ATTR); // ) 
	matchToken(KW_T, THEN); //then
	matchToken(LBR_T, NO_ATTR); // {
	opt_statements();
	matchToken(RBR_T, NO_ATTR); // }
	matchToken(KW_T, ELSE); // else
	matchToken(LBR_T, NO_ATTR); // {
	opt_statements();
	matchToken(RBR_T, NO_ATTR); // }
	matchToken(EOS_T, NO_ATTR); // ; 
	printf("PLATY: Selection statement parsed\n");
}

/*************************************************************
 * iteration_statement
 * BNF: <iteration_statement> -> WHILE <pre-condition> (<conditional_expression>) DO { <statements>};
 * FIRST(<pre-condition>) = { KW_T(TRUE), KW_T(FALSE) } 
 ************************************************************/
void iteration_statement(void) {
	matchToken(KW_T, WHILE); //while
	pre_condition();
	matchToken(LPR_T, NO_ATTR); // (
	conditional_expression();
	matchToken(RPR_T, NO_ATTR); // )
	matchToken(KW_T, DO); //do
	matchToken(LBR_T, NO_ATTR); // {
	statements();
	matchToken(RBR_T, NO_ATTR); // }
	matchToken(EOS_T, NO_ATTR); // ;
	printf("PLATY: Iteration statement parsed\n");
}

/*************************************************************
 * pre_condition
 * BNF: <pre_condition> -> TRUE | FALSE
 * FIRST(<pre-condition>) = { KW_T(TRUE), KW_T(FALSE) } 
 ************************************************************/
void pre_condition(void) {
	switch (lookahead.code) {
	case KW_T:
		if (lookahead.attribute.get_int == TRUE) { //if true
			matchToken(KW_T, TRUE);
		}
		else if (lookahead.attribute.get_int == FALSE) { //if false
			matchToken(KW_T, FALSE);
		}
		else { //if keyword isn't true or false
			printError();
		}
		break;
	default: //if not a keyword
		printError();
		break;
	}
}

/*************************************************************
 * input_statement
 * BNF: <input_statement> -> INPUT (<variable list>);
 * FIRST(<input_statement>) = { KW_T(INPUT) } 
 ************************************************************/
void input_statement(void) {
	matchToken(KW_T, INPUT); //input
	matchToken(LPR_T, NO_ATTR); // (
	variable_list();
	matchToken(RPR_T, NO_ATTR); // )
	matchToken(EOS_T, NO_ATTR); //;
	printf("PLATY: Input statement parsed\n");
}

/*************************************************************
 * variable_list
 * BNF: <variable_list> -> <variable_identifier> <variable_listPrime>
 * FIRST(<variable_list>) = { AVID_T, SVID_T } 
 ************************************************************/
void variable_list(void) {
	variable_identifier();
	variable_listPrime();
	printf("PLATY: Variable list parsed\n");
}


/*************************************************************
 * variable_listPrime
 * BNF: <variable_listPrime> -> , <variable_identifier> <variable_listPrime> | e
 * FIRST(<variable_listPirme>) = { COM_T, e }
 ************************************************************/
void variable_listPrime(void) {
	switch (lookahead.code) {
	case COM_T:
		matchToken(COM_T, NO_ATTR); // ,
		variable_identifier();
		variable_listPrime();
		break;
	} //else e
}

/*************************************************************
 * variable_identifier
 * BNF: <variable_identifier> -> AVID_T | SVID_T 
 * FIRST(<variable_identifier>) = { AVID_T, SVID_T}
 ************************************************************/
void variable_identifier(void) {
	switch (lookahead.code) {
	case AVID_T: //avid
		matchToken(AVID_T, NO_ATTR);
		break;
	case SVID_T: //svid
		matchToken(SVID_T, NO_ATTR);
		break;
	default: //error
		printError();
		break;
	}
}


/*************************************************************
 * output_statement
 * BNF: <output_statement> -> OUTPUT (<output_list>);
 * FIRST(<output_statement>) = { KW_T(OUTPUT) } 
 ************************************************************/
void output_statement(void) {
	matchToken(KW_T, OUTPUT); //output
	matchToken(LPR_T, NO_ATTR); // (
	output_list();
	matchToken(RPR_T, NO_ATTR); // )
	matchToken(EOS_T, NO_ATTR); // ; 
	printf("PLATY: Output statement parsed\n");
}

/*************************************************************
 * output_list
 * BNF: <output_list> -> <opt_variable_list> | STR_T;
 * FIRST(<output list>) = { AVID_T, SVID_T, STR_T, e }
 ************************************************************/
void output_list(void) {
	switch (lookahead.code) {
	case STR_T:
		matchToken(STR_T, NO_ATTR);
		printf("PLATY: Output list (string literal) parsed\n");
		break;
	default:
		opt_variable_list();
		break;
	}
}

/*************************************************************
 * opt_variable_list
 * BNF: <opt_variable_list> -> <variable_list> | e
 * FIRST(<opt_variable list>) = { SVID_T, AVID_T, e }
 ************************************************************/
void opt_variable_list(void) {
	switch (lookahead.code) {
	case AVID_T: case SVID_T:
		variable_list();
		break;
	default:  //else e
		printf("PLATY: Output list (empty) parsed\n");
		break;
	}
}

/*************************************************************
 * arithmetic_expression
 * BNF: <arithmetic_expression> ->  <unary_arithmetic_expression> | <additive_arithmetic_expression>
 * FIRST(<arithmetic_expression>) = { ART_OP_T(ADD), ART_OP_T(SUB), AVID_T, FPL_T, INL_T, LPR_T } 
 ************************************************************/
void arithmetic_expression(void) {
	switch (lookahead.code) {
	case ART_OP_T:
		if (lookahead.attribute.arr_op == ADD || lookahead.attribute.arr_op == SUB) {
			unary_arithmetic_expression();
		}
		else {
			printError();
			break;
		}
		printf("PLATY: Arithmetic expression parsed\n");
		break;
	case AVID_T: case FPL_T: case INL_T: case LPR_T:
		additive_arithmetic_expression();
		printf("PLATY: Arithmetic expression parsed\n");
		break;
	default:
		printError();
		break;
	}
}

/*************************************************************
 * unary_arithmetic_expression
 * BNF: <unary_arithmetic_expression> -> - <primary_arithmetic_expression> | + <primary_arithmetic_expression>
 * FIRST(<unary_arithmetic_expression>) = { ART_OP_T(ADD), ART_OP_T(SUB) } 
 ************************************************************/
void unary_arithmetic_expression(void) {
	switch (lookahead.code) {
	case ART_OP_T:
		if (lookahead.attribute.arr_op == SUB 
			|| lookahead.attribute.arr_op == ADD) {
			matchToken(ART_OP_T, lookahead.attribute.arr_op);
			primary_arithmetic_expression();
		}
		else {
			printError();
		}
		break;
	}
	printf("PLATY: Unary arithmetic expression parsed\n");
}

/*************************************************************
 * additive_arithmetic_expression
 * BNF: <additive_arithmetic_expression> -> <multiplicative_arithmetic_expression> <additive_arithmetic_expressionPrime>
 * FIRST(<additive arithmetic expression>) = { AVID_T, FPL_T, INL_T, LPR_T } 
 ************************************************************/
void additive_arithmetic_expression(void) {
	multiplicative_arithmetic_expression();
	additive_arithmetic_expressionPrime();
}

/*************************************************************
 * additive_arithmetic_expressionPrime
 * BNF: <additive_arithmetic_expressionPrime> -> + <multiplicative_arithmetic_expression> <additive_arithmetic_expressionPrime> 
												 | - <multiplicative_arithmetic_expression> <additive_arithmetic_expressionPrime> 
												 | e
 * FIRST(<additive_arithmetic_expressionPrime>) = { ART_OP_T(ADD), ART_OP_T(SUB), e }
 ************************************************************/
void additive_arithmetic_expressionPrime(void) {
	if (lookahead.code == ART_OP_T) {
		if (lookahead.attribute.arr_op == ADD
			|| lookahead.attribute.arr_op == SUB) {
			matchToken(ART_OP_T, lookahead.attribute.arr_op);
			multiplicative_arithmetic_expression();
			additive_arithmetic_expressionPrime();
			printf("PLATY: Additive arithmetic expression parsed\n");
		}
	}
}

/*************************************************************
 * multiplicative_arithmetic_expression
 * BNF: <multiplicative_arithmetic_expression> -> <primary_arithmetic_expression> <multiplicative_arithmetic_expressionPrime>
 * FIRST(<multiplicative_arithmetic_expression>) = { AVID_T, FPL_T, INL_T, LPR_T }
 ************************************************************/
void multiplicative_arithmetic_expression(void) {
	primary_arithmetic_expression();
	multiplicative_arithmetic_expressionPrime();
}

/*************************************************************
 * multiplicative_arithmetic_expressionPrime
 * BNF:<multiplicative_arithmetic_expressionPrime> -> * <primary_arithmetic_expression> <multiplicative_arithmetic_expressionPrime> 
													  | / <primary_arithmetic_expression> <multiplicative_arithmetic_expressionPrime 
													  | e
 * FIRST(<multiplicative_arithmetic_expressionPrime>) = { ART_OP_T(MUL), ART_OP_T(DIV), e }
 ************************************************************/
void multiplicative_arithmetic_expressionPrime(void) {
	if (lookahead.code == ART_OP_T) {
		if (lookahead.attribute.arr_op == MUL
			|| lookahead.attribute.arr_op == DIV) {
			matchToken(ART_OP_T, lookahead.attribute.arr_op);
			primary_arithmetic_expression();
			multiplicative_arithmetic_expressionPrime();
			printf("PLATY: Multiplicative arithmetic expression parsed\n");
		}
	} //else e
}

/*************************************************************
 * primary_arithmetic_expression
 * BNF: <primary_arithmetic_expression> -> AVID_T | FPL_T | INL_T | (<arithmetic_expression>)
 * FIRST(<primary_arithmetic_expression>) = { AVID_T, FPL_T, INL_T, LPR_T }
 ************************************************************/
void primary_arithmetic_expression(void) {
	switch (lookahead.code) {
	case AVID_T: case FPL_T: case INL_T:
		matchToken(lookahead.code, NO_ATTR);
		printf("PLATY: Primary arithmetic expression parsed\n");
		break;
	case LPR_T:
		matchToken(LPR_T, NO_ATTR);
		arithmetic_expression();
		matchToken(RPR_T, NO_ATTR);
		printf("PLATY: Primary arithmetic expression parsed\n");
		break;
	default:
		printError();
		break;
	}
}

/*************************************************************
 * string_expression
 * BNF: <string_expression> -> <primary_string_expression> <string_expressionPrime>
 * FIRST(<string_expression>) = { SVID_T, STR_T } 
 ************************************************************/
void string_expression(void) {
	primary_string_expression();
	string_expressionPrime();
	printf("PLATY: String expression parsed\n");
}

/*************************************************************
 * string_expressionPrime
 * BNF: <string_expressionPrime> -> $$ <primary_string_expression> <string_expressionPrime> | e
 * FIRST(<string_expressionPrime>) = { SCC_OP_T, e }
 ************************************************************/
void string_expressionPrime(void) {
	if (lookahead.code == SCC_OP_T) {
		matchToken(SCC_OP_T, NO_ATTR);
		primary_string_expression();
		string_expressionPrime();
	}
}

/*************************************************************
 * primary_string_expression
 * BNF: <primary_string_expression> ->  SVID_T  |  STR_T
 * FIRST(<primary_string_expression>) = { SVID_T, STR_T }
 ************************************************************/
void primary_string_expression(void) {
	switch (lookahead.code) {
	case SVID_T: case STR_T:
		matchToken(lookahead.code, NO_ATTR);
		printf("PLATY: Primary string expression parsed\n");
		break;
	default:
		printError();
		break;
	}
}

/*************************************************************
 * conditional_expression
 * BNF: <conditional_expression> -> <logical_or_expression>
 * FIRST(<conditional_expression>) = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }
 ************************************************************/
void conditional_expression(void) {
	logical_or_expression();
	printf("PLATY: Conditional expression parsed\n");
}


/*************************************************************
 * logical_or_expression
 * BNF: <logical_or_expression> -> <logical_and_expression> <logical_or_expressionPrime>
 * FIRST(<logical_or_expression>) = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }
 ************************************************************/
void logical_or_expression(void) {
	logical_and_expression();
	logical_or_expressionPrime();
}

/*************************************************************
 * logical_or_expressionPrime
 * BNF: <logical_or_expressionPrime> -> _OR_ <logical_and_expression> <logical_or_expressionPrime> | e
 * FIRST(<logical_or_expressionPrime>) = { LOG_OP_T(OR), e }
 ************************************************************/
void logical_or_expressionPrime(void) {
	if (lookahead.code == LOG_OP_T) {
		if (lookahead.attribute.log_op == OR) {
			matchToken(LOG_OP_T, OR);
			logical_and_expression();
			logical_or_expressionPrime();
			printf("PLATY: Logical OR expression parsed\n");
		}
	} //else e 
}

/*************************************************************
 * logical_and_expression
 * BNF: <logical_and_expression> -> <logical_not_expression> <logical_and_expressionPrime>
 * FIRST(<logical_and_expression>) = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }
 ************************************************************/
void logical_and_expression(void) {
	relational_expression();
	logical_and_expressionPrime();
}

/*************************************************************
 * logical_and_expressionPrime
 * BNF: <logical_and_expressionPrime> -> _AND_ <logical_not_expression> <logical_and_expressionPrime> | e
 * FIRST(<logical_and_expressionPrime>) = { LOG_OP_T(AND), e }
 ************************************************************/
void logical_and_expressionPrime(void) {
	if (lookahead.code == LOG_OP_T) {
		if (lookahead.attribute.log_op == AND) {
			matchToken(LOG_OP_T, AND);
			logical_not_expression();
			logical_and_expressionPrime();
			printf("PLATY: Logical AND expression parsed\n");
		}
	} //else e
}

/*************************************************************
 * logical_not_expression
 * BNF:<logical_not_expression> ->  _NOT_ <relational_expression> | <relational_expression>
 * FIRST(<logical_not_expression >) = { LOG_OP_T(NOT), e }
 ************************************************************/
void logical_not_expression (void){
	if (lookahead.code == LOG_OP_T) {
		if (lookahead.attribute.log_op == NOT) {
			matchToken(LOG_OP_T, NOT);
			relational_expression();
			logical_not_expression();
			printf("PLATY: Logical NOT expression parsed\n");
		}
	}
	else {
		relational_expression();
	}
}

/*************************************************************
 * relational_expression
 * BNF: <relational_expression> -> <primary_a_relational_expression> <operator_list> <primary_a_relational_expression>
								   | <primary_s_relational_expression> <operator_list> <primary_s_relational_expression>
 * FIRST(<relational_expression>) = { AVID_T, FPL_T, INL_T, STR_T, SVID_T }
 ************************************************************/
void relational_expression(void) {
	switch (lookahead.code) {
	case AVID_T: case FPL_T: case INL_T:
		primary_a_relational_expression();
		operator_list();
		primary_a_relational_expression();
		break;
	case SVID_T: case STR_T:
		primary_s_relational_expression();
		operator_list();
		primary_s_relational_expression();
		break;
	default:
		printError();
		break;
	}
	printf("PLATY: Relational expression parsed\n");
}

/*************************************************************
 * operator_list
 * BNF: <operator_list> ->  == | <> | > | <
 * FIRST(<operator_list>) = { REL_OP_T(EQ), REL_OP_T(NE), REL_OP_T(GT), REL_OP_T(LT) }
 ************************************************************/
void operator_list(void) {
	if (lookahead.code == REL_OP_T) {
		matchToken(REL_OP_T, lookahead.attribute.log_op);
	}
	else {
		printError();
	}
}

/*************************************************************
 * primary_a_relational_expression
 * BNF: <primary_a_relational_expression> -> AVID_T | FPL_T | INL_T
 * FIRST(<primary_a_relational_expression>) = { AVID_T, FPL_T, INL_T }
 ************************************************************/
void primary_a_relational_expression(void) {
	switch (lookahead.code) {
	case AVID_T: case FPL_T: case INL_T:
		matchToken(lookahead.code, NO_ATTR);
		break;
	default: //else error
		printError();
		break;
	}
	printf("PLATY: Primary a_relational expression parsed\n");
}

/*************************************************************
 * primary_s_relational_expression
 * BNF: <primary_s_relational_expression> -> <primary_string_expression>
 * FIRST(<primary_s_relational_expression>) = { SVID_T, STR_T }
 ************************************************************/
void primary_s_relational_expression(void) {
	primary_string_expression();
	printf("PLATY: Primary s_relational expression parsed\n");
}