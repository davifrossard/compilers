%option outfile="scanner.c"
%option noyywrap
%option nounput
%option noinput
%option yylineno
%x MLC
%{
#include "bt.h"
#include "parser.h"
#include "string.h"

char* copyval(char* data)
{
  char* res = malloc(sizeof(char) * (strlen(data) + 1));
  strcpy(res, data);
  return res;
}
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
else      { yylval = copyval(yytext); return ELSE; }
if        { yylval = copyval(yytext); return IF; }
input     { yylval = copyval(yytext); return INPUT; }
int       { yylval = copyval(yytext); return INT; }
output    { yylval = copyval(yytext); return OUTPUT; }
return    { yylval = copyval(yytext); return RETURN; }
void      { yylval = copyval(yytext); return VOID; }
while     { yylval = copyval(yytext); return WHILE; }
write     { yylval = copyval(yytext); return WRITE; }
"+"       { yylval = copyval(yytext); return PLUS; }
"-"       { yylval = copyval(yytext); return MINUS; }
"*"       { yylval = copyval(yytext); return TIMES; }
"/"       { yylval = copyval(yytext); return OVER; }
"<"       { yylval = copyval(yytext); return LT; }
"<="      { yylval = copyval(yytext); return LE; }
">"       { yylval = copyval(yytext); return GT; }
">="      { yylval = copyval(yytext); return GE; }
"=="      { yylval = copyval(yytext); return EQ; }
"!="      { yylval = copyval(yytext); return NEQ; }
"="       { yylval = copyval(yytext); return ASSIGN; }
";"       { yylval = copyval(yytext); return SEMI; }
","       { yylval = copyval(yytext); return COMMA; }
"("       { yylval = copyval(yytext); return LPAREN; }
")"       { yylval = copyval(yytext); return RPAREN; }
"["       { yylval = copyval(yytext); return LBRACK; }
"]"       { yylval = copyval(yytext); return RBRACK; }
"{"       { yylval = copyval(yytext); return LBRACE; }
"}"       { yylval = copyval(yytext); return RBRACE; }
{number}  { yylval = copyval(yytext); return NUM; }
{string}  { yylval = copyval(yytext); return STRING; }
{id}      { yylval = copyval(yytext); return ID; }
.         { printf("SCANNING ERROR (%d): Unknown symbol %s\n", yylineno, yytext); exit(0); }
%%
