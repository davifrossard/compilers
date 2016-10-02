%option outfile="scanner.c"
%option noyywrap
%option nounput
%option noinput
%option yylineno
id [a-zA-Z][a-zA-Z0-9]*
%{
#include "token.h"
extern int lval;
%}
number [0-9]+
string \"[^"]*\"
ignore "//".*|[ \t]*|"{"[^}]*"}"
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
"+"       { return PLUS; }
"-"       { return MINUS; }
"*"       { return TIMES; }
"/"       { return DIVISION; }
":="      { return ASSIGN; }
"="       { return EQ; }
">"       { return GT; }
"<"       { return LT; }
">="      { return GE; }
"<="      { return LE; }
";"       { return END; }
"("       { return LPAREN; }
")"       { return RPAREN; }
{number}  { return NUMBER; }
{varname} { return ID; }
<<EOF>>   { return ENDFILE; }
"\n"	  { return NEWLINE; }
.         { return UNKNOWN; }
%%
