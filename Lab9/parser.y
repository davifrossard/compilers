
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
#include "tables.h"
extern int yylineno;
extern char* yytext;
int yylex(void);
void yyerror (char const *s)
{
	printf("PARSE ERROR (%d): %s\n", yylineno,s);
	exit(0);
}


LitTable *lits_table;
SymTable *syms_table;

void safe_add_var(char* varname, int lineno)
{
    int idx = lookup_var(syms_table, varname);
    if(idx == -1)
    {
        add_var(syms_table, varname, lineno);
    }
    else
    {
        printf("SEMANTIC ERROR (%d): variable '%s' already declared at line %d.\n",
               lineno, varname, get_line(syms_table, idx));
        exit(0);
    }
}

void get_var(char* varname, int lineno)
{
    int idx = lookup_var(syms_table, varname);
    if(idx == -1)
    {
        printf("SEMANTIC ERROR (%d): variable '%s' was not declared.\n",
                lineno, varname);
        exit(0);
    }
}

%}
%union {
  int intval;
  char strval[50];
}

%token <intval> IGNORE IF THEN ELSE END REPEAT UNTIL READ WRITE PUTS PLUS MINUS TIMES
OVER NUMBER ENDOF UNKNOWN EQ NEWLINE LPAREN RPAREN ASSIGN LESSTHAN SCOLON
%token <strval> ID STRING
INT
%left EQ LESSTHAN
%left PLUS MINUS  /* Ops associativos a esquerda. */
%left TIMES OVER  /* Mais para baixo, maior precedencia. */

%%
line: %empty |
stmt_seq
;

expr: NUMBER
| expr PLUS expr
| expr MINUS expr
| expr TIMES expr
| expr OVER expr
| expr EQ expr
| expr LESSTHAN expr
| LPAREN expr RPAREN
| ID	{ get_var($1, yylineno); }
;

stmt_seq: stmt_seq stmt
| stmt
;

stmt: if_stmt
| read_stmt
| assign_stmt
| write_stmt
| repeat_stmt
| puts_stmt
| decl_var
;

repeat_stmt: REPEAT stmt_seq UNTIL expr
;

read_stmt: READ ID SCOLON { get_var($2, yylineno); }
;

assign_stmt: ID ASSIGN expr SCOLON { get_var($1, yylineno); }
;

write_stmt: WRITE expr SCOLON
;

puts_stmt: PUTS STRING SCOLON { add_literal(lits_table, $2); printf("PUTS: %s.\n", $2); }
;

if_stmt: IF expr THEN stmt_seq ELSE stmt_seq END
| IF expr THEN stmt_seq END
;

decl_var: INT ID SCOLON { safe_add_var($2, yylineno); }
;

%%

int main() {
  lits_table = create_lit_table();
  syms_table = create_sym_table();
  yyparse();
  printf("\n\n");

  print_lit_table(lits_table);
  printf("\n\n");

  print_sym_table(syms_table);
  printf("\n\n");

  return 0;
}
