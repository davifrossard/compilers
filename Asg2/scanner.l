%option outfile="scanner.c"
%option noyywrap
%option nounput
%option noinput
%option yylineno
%x MLC
%{
#include "parser.h"
%}
number [0-9]+
string \"[^"]*\"
ignore "//".*|[ \t\r\n]*
id [a-zA-Z][a-zA-Z0-9]*
%%
"/*"      { BEGIN(MLC); }
<MLC>"*/" { BEGIN(INITIAL); }
<MLC>.|\n { }
{ignore}  { }
else      { return ELSE; }
if        { return IF; }
input     { return INPUT; }
int       { return INT; }
output    { return OUTPUT; }
return    { return RETURN; }
void      { return VOID; }
while     { return WHILE; }
write     { return WRITE; }
"+"       { return PLUS; }
"-"       { return MINUS; }
"*"       { return TIMES; }
"/"       { return OVER; }
"<"       { return LT; }
"<="      { return LE; }
">"       { return GT; }
">="      { return GE; }
"=="      { return EQ; }
"!="      { return NEQ; }
"="       { return ASSIGN; }
";"       { return SEMI; }
","       { return COMMA; }
"("       { return LPAREN; }
")"       { return RPAREN; }
"["       { return LBRACK; }
"]"       { return RBRACK; }
"{"       { return LBRACE; }
"}"       { return RBRACE; }
{number}  { return NUM; }
{string}  { return STRING; }
{id}      { return ID; }
.         { printf("SCANNING ERROR (%d): Unknown symbol %s\n", yylineno, yytext); exit(0); }
%%
