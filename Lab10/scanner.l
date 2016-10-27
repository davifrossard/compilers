%option outfile="scanner.c"
%option noyywrap
%option nounput
%option noinput
%option yylineno
id [a-zA-Z][a-zA-Z0-9]*
%{
#include "parser.h"
extern int lval;
%}
number [0-9]+
real [0-9]*\.[0-9]+
string \"[^"]*\"
ignore "//".*|[ \t\n]*|"{"[^}]*"}"
varname [a-zA-Z][a-zA-Z0-9]*
%%
{ignore}  { }
print     { return PRINT; }
int       { return INT; }
real      { return REAL; }
string    { return TSTRING; }
bool      { return BOOL; }
true      { return TRUE; }
false     { return FALSE; }
";"       { return SCOLON; }
"+"       { return PLUS; }
"-"       { return MINUS; }
"*"       { return TIMES; }
"/"       { return OVER; }
"="       { return EQ; }
":="      { return ASSIGN; }
"<"       { return LESSTHAN; }
"("       { return LPAREN; }
")"       { return RPAREN; }
","       { return COMMA; }
{string}  { strcpy(yylval.strval, yytext); return STRING;  }
{real}    { yylval.floatval = atof(yytext); return RNUMBER; }
{number}  { yylval.intval = atoi(yytext); return NUMBER; }
{varname} { strcpy(yylval.strval, yytext); return ID; }
.         { printf("SCANNING ERROR (%d): Unknown symbol %s\n", yylineno, yytext); exit(0); }
%%
