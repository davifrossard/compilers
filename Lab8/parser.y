
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
#include <stdlib.h>
#include <math.h>
#include "bt.h"
extern int yylineno;
extern char* yytext;
int yylex(void);
void yyerror (char const *s)
{
	printf("PARSE ERROR (%d): %s\n", yylineno,s);
	exit(0);
}
%}
%define api.value.type {BT*} // Tipo da variavel yylval
%token IGNORE IF THEN ELSE END REPEAT UNTIL READ WRITE PLUS MINUS TIMES OVER NUMBER ID ENDOF UNKNOWN EQ NEWLINE LPAREN RPAREN ASSIGN LESSTHAN SCOLON
%left EQ LESSTHAN
%left PLUS MINUS  /* Ops associativos a esquerda. */
%left TIMES OVER  /* Mais para baixo, maior precedencia. */

%%
line: %empty |
stmt_seq
;

expr: NUMBER 	{ $$ = $1; }
| expr PLUS expr	{ $$ = create_node("expr"); add_child($$, $1); add_child($$, $2); add_child($$, $3); }
| expr MINUS expr	{ $$ = create_node("expr"); add_child($$, $1); add_child($$, $2); add_child($$, $3); }
| expr TIMES expr	{ $$ = create_node("expr"); add_child($$, $1); add_child($$, $2); add_child($$, $3); }
| expr OVER expr	{ $$ = create_node("expr"); add_child($$, $1); add_child($$, $2); add_child($$, $3); }
| expr EQ expr	{ $$ = create_node("expr"); add_child($$, $1); add_child($$, $2); add_child($$, $3); }
| expr LESSTHAN expr	{ $$ = create_node("expr"); add_child($$, $1); add_child($$, $2); add_child($$, $3); }
| LPAREN expr RPAREN	{ $$ = create_node("expr"); add_child($$, $1); add_child($$, $2); add_child($$, $3); }
| ID	{ $$ = $1; }
;

stmt_seq: stmt_seq stmt
| stmt
;

stmt: if_stmt
| read_stmt
| assign_stmt
| write_stmt
| repeat_stmt
;

repeat_stmt: REPEAT stmt_seq UNTIL expr
;

read_stmt: READ ID SCOLON
;

assign_stmt: ID ASSIGN expr SCOLON
;

write_stmt: WRITE expr SCOLON
;

if_stmt: IF expr THEN stmt_seq ELSE stmt_seq END
| IF expr THEN stmt_seq END
;

%%

int main() {
    yydebug = 0; // Toggle this variable to enter debug mode.

    if (yyparse() == 0) {
        //printf("AST of given expression:\n");
        //print_tree(tree);
        print_dot(tree);
        free_tree(tree);
    }

    return 0;
}
