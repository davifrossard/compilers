
/* Simple integer arithmetic calculator implemented according to the
 * following EBNF grammar:
 *
 * <expr> -> <term> { <addop> <term> }
 * <addop> -> + | -
 * <term> -> <factor> { <mulop> <factor> }
 * <mulop> -> *
 * <factor> -> ( <expr> ) | NUMBER
 *
 * Input: a line of text from stdin.
 * Output: "Error" or the computed result.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "token.h"

TokenType token;  // Global token type variable.
int lval; // Global variable for number tokens.
extern int yylineno;
extern char* yytext;

TokenType yylex(); // Flex function.

void error(void) {
    printf("SCANNING ERROR (%d): Unknown symbol %s\n", yylineno, yytext);
    exit(1);
}

void match(int expected_token) {
    if (token == expected_token) {
        token = yylex();
    } else {
        error();
    }
}

void program() {
    while(token != ENDOF)
    {
        //statement();
    }
}

void statement() {
    switch(token){
        case IF:
            //if_statement();
            break;
        case REPEAT:
            //repeat_statement();
            break;
        case ID:
            //assign_statement();
            break;
        case READ:
            //read_statement();
            break;
        case WRITE:
            //write_statement();
            break;
        default:
            error();
    }
}

int main(void) {
    token = yylex();
    program();
    return 0;
}
