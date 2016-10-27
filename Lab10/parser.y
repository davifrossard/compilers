
/* Options to bison */
// File name of generated parser.
%output "parser.c"
// Produces a 'parser.h'
%defines "parser.h"
// Give proper error messages when a syntax error is found.
%define parse.error verbose
// Enable LAC (lookahead correction) to improve syntax error handling.
%define parse.lac full
%code requires {#include "tables.h"}
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

void safe_add_var(char* varname, type_names type, int lineno)
{
    int idx = lookup_var(syms_table, varname);
    if(idx == -1)
    {
        add_var(syms_table, varname, lineno, type);
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

type_names get_var_type(char* varname, int lineno)
{
    int idx = lookup_var(syms_table, varname);
    if(idx == -1)
    {
        printf("SEMANTIC ERROR (%d): variable '%s' was not declared.\n",
                lineno, varname);
        exit(0);
    }
    else
    {
        return get_type(syms_table, idx);
    }
}

type_names get_sum_result(type_names x, type_names y)
{
    if(x == T_INT || x == T_BOOL)
    {
        return y;
    }
    else if(x == T_REAL)
    {
        if(y == T_STRING)
            return T_STRING;
        else
            return T_REAL;
    }
    else
        return T_STRING;
}

type_names get_fact_result(type_names x, type_names y)
{
    if(x == T_INT && y == T_INT)
        return T_INT
    else((x == T_INT || y))

}

type_names _LAST_TYPE_READ;
%}

%union {
  int intval;
  float floatval;
  type_names typeval;
  char strval[50];
}

%token <intval> IGNORE PRINT PLUS MINUS TIMES OVER NUMBER RNUMBER ENDOF UNKNOWN EQ
NEWLINE LPAREN RPAREN ASSIGN LESSTHAN SCOLON COMMA INT REAL TSTRING BOOL TRUE
FALSE

%token <strval> ID STRING

%type <typeval> expr

%left EQ LESSTHAN
%left PLUS MINUS
%left TIMES OVER

%%
line: %empty |
stmt_seq
;

expr: NUMBER	{ $$ = T_INT; }
| RNUMBER	{ $$ = T_REAL; }
| TRUE	{ $$ = T_BOOL; }
| FALSE	{ $$ = T_BOOL; }
| STRING	{ $$ = T_STRING; }
| expr PLUS expr	{ $$ = get_sum_result($1, $3); }
| expr MINUS expr
| expr TIMES expr
| expr OVER expr
| expr EQ expr
| expr LESSTHAN expr
| LPAREN expr RPAREN    { $$ = $1; }
| ID	{ get_var($1, yylineno); $$ = get_var_type($1, yylineno); }
;

stmt_seq: stmt_seq stmt
| stmt
;

stmt: assign_stmt
| decl_var
| print_stmt
;

assign_stmt: ID ASSIGN expr SCOLON { get_var($1, yylineno); }
;

print_stmt: PRINT ID SCOLON { get_var($2, yylineno); printf("PRINT: %s.\n", $2); }
;

ids: ID COMMA ids  { safe_add_var($1, _LAST_TYPE_READ, yylineno); printf("%d\n", _LAST_TYPE_READ); }
| ID { safe_add_var($1, _LAST_TYPE_READ, yylineno); printf("%d\n", _LAST_TYPE_READ); }
;

decl_var: INT { _LAST_TYPE_READ = T_INT; } ids SCOLON
| REAL { _LAST_TYPE_READ = T_REAL; } ids SCOLON
| TSTRING { _LAST_TYPE_READ = T_STRING; } ids SCOLON
| BOOL { _LAST_TYPE_READ = T_BOOL; } ids SCOLON
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
