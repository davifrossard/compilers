
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

void error();
void match(int);
void program();
void statement();
void mul_op();
void add_op();
void term();
void if_statement();
void repeat_statement();
void assign_statement();
void read_statement();
void write_statement();
void factor();
void term();
void compare_op();
void expression();
void simple_expression();

void error() {
    printf("SCANNING ERROR (%d): Unknown symbol %s\n", yylineno, yytext);
    exit(1);
}

void match(int expected_token) {
    if (token == expected_token) {
        token = yylex();
    } 
    else {
        error();
    }
}

void program() {
    //printf("Program - %s\n", yytext);
    while(token != ENDFILE) {
        statement();
    }
    printf("PARSE SUCESSFUL!\n");
}

void statement() {
    //printf("Statement - %s\n", yytext);
    switch(token){
        case IF:
            if_statement();
            break;
        case REPEAT:
            repeat_statement();
            break;
        case ID:
            assign_statement();
            break;
        case READ:
            read_statement();
            break;
        case WRITE:
            write_statement();
            break;
        case NEWLINE:
            match(NEWLINE);
            break;
        default:
            error();
            break;
    }
}

void add_op() {
    //printf("Add Op - %s\n", yytext);
    if(token == PLUS)
        match(PLUS);
    else if(token == MINUS)
        match(MINUS);
}

void mul_op() {
    //printf("Mul Op - %s\n", yytext);
    if(token == TIMES) {
        match(TIMES);
    }
    else if(token == DIVISION) {
        match(DIVISION);
    }
}

void factor() {
    //printf("Factor - %s\n", yytext);
    if(token == NUMBER) {
        match(NUMBER);
    }
    else if(token == ID) {
        match(ID);
    }
    else if(token == LPAREN) {
        match(LPAREN);
        expression();
        match(RPAREN);
    }
}

void expression() {
    //printf("Expression - %s\n", yytext);
    simple_expression();
    if(token == LT || token == EQ) {
        compare_op();
        simple_expression();
    }
}

void simple_expression() {
    //printf("Simple Expression - %s\n", yytext);
    term();
    while(token == PLUS || token == MINUS) {
        add_op();
        term();
    }
}
    
void compare_op() {
    //printf("Compare Op - %s\n", yytext);
    if(token == EQ) {
        match(EQ);
    }
    else if(token == LT) {
        match(LT);
    }
}

void term() {
    //printf("Term - %s\n", yytext);
    factor();
    while(token == TIMES || token == DIVISION) {
        mul_op();
        factor();
    }
}

void if_statement() {
    //printf("If Statement - %s\n", yytext);
    match(IF);
    expression();
    match(THEN);
    while(token != ELSE && token != END) {
        statement();
    }
    if(token == ELSE) {
        match(ELSE);
        while(token != END)
        {
            statement();
        }
    }
    match(END);
}

void read_statement() {
    //printf("Read Statement - %s\n", yytext);
    match(READ);
    match(ID);
    match(END);
}

void write_statement() {
    //printf("Write Statement - %s\n", yytext);
    match(WRITE);
    expression();
    match(END);
}

void assign_statement() {
    //printf("Assign Statement - %s\n", yytext);
    match(ID);
    match(ASSIGN);
    expression();
    match(END);
}

void repeat_statement() {
    //printf("Repeat Statement - %s\n", yytext);
    match(REPEAT);
    while(token != UNTIL) {
        statement();
    }
    match(UNTIL);
    expression();
}

int main(void) {
    token = yylex();
    program();
    return 0;
}
