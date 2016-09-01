
/* Options to bison */
// File name of generated parser.
%output "parser.c"
// Produces a 'parser.h'
%defines "parser.h"
// Give proper error messages when a syntax error is found.
%define parse.error verbose
// Enable LAC (lookahead correction) to improve syntax error handling.
%define parse.lac full

%{
#include <stdio.h>
#include <math.h>

int yylex(void);
void yyerror(char const *s);
%}
%define api.value.type {double} // Tipo da variavel yylval
%token NUMBER HEX IF ELSE EQ
%left '+' '-'  /* Ops associativos a esquerda. */
%left '*' '/'  /* Mais para baixo, maior precedencia. */
%left '^'
%precedence '(' ')'
%precedence IF
%precedence ELSE

%%
lines: %empty
| lines stmt '\n'     	{ printf(">> %.2f\n", $2); }
;
expr: NUMBER 				{ $$ = $1; }
| HEX					{ $$ = $1; }
| expr '+' expr        			{ $$ = $1 + $3; }
| '-' expr 				{ $$ = -$2; }
| expr '-' expr				{ $$ = $1 + (-$3); }
| expr '*' expr         		{ $$ = $1 * $3; }
| expr '/' expr         		{ $$ = $1 / $3; } 
| expr '^' expr				{ $$ = pow($1, $3); }
| expr EQ expr 				{ $$ = $1 == $3; } 
; 
stmt: IF '(' expr ')' stmt ELSE stmt 	{ $$ = $3 ? $5 : $7; } 
| IF '(' expr ')' stmt 		     	{ $$ = $3 ? $5 : 0; }
| expr				     	{ $$ = $1; }
;
%%
