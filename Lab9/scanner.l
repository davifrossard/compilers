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
string \"[^"]*\"
ignore "//".*|[ \t\n]*|"{"[^}]*"}"
varname [a-zA-Z][a-zA-Z0-9]*
%%
{ignore}  { }
if        { return IF; }
then      { return THEN; }
else      { return ELSE; }
end       { return END; }
repeat    { return REPEAT; }
until     { return UNTIL; }
read      { return READ; }
write     { return WRITE; }
puts      { return PUTS; }
int       { return INT; }
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
{string}  { strcpy(yylval.strval, yytext); return STRING; }
{number}  { yylval.intval = atoi(yytext); return NUMBER; }
{varname} { strcpy(yylval.strval, yytext); return ID; }
.         { printf("SCANNING ERROR (%d): Unknown symbol %s\n", yylineno, yytext); exit(0); }
%%
