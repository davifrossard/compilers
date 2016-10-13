%option outfile="scanner.c"
%option noyywrap
%option nounput
%option noinput

%{
#include "bt.h"
#include "parser.h"
%}
number [0-9]+
string \"[^"]*\"
ignore "//".*|[ \t\n]*|"{"[^}]*"}"
varname [a-zA-Z][a-zA-Z0-9]*
%%
{ignore}  { }
if        { yylval = create_node(yytext); return IF; }
then      { yylval = create_node(yytext); return THEN; }
else      { yylval = create_node(yytext); return ELSE; }
end       { yylval = create_node(yytext); return END; }
repeat    { yylval = create_node(yytext); return REPEAT; }
until     { yylval = create_node(yytext); return UNTIL; }
read      { yylval = create_node(yytext); return READ; }
write     { yylval = create_node(yytext); return WRITE; }
";"       { yylval = create_node(yytext); return SCOLON; }
"+"       { yylval = create_node(yytext); return PLUS; }
"-"       { yylval = create_node(yytext); return MINUS; }
"*"       { yylval = create_node(yytext); return TIMES; }
"/"       { yylval = create_node(yytext); return OVER; }
"="       { yylval = create_node(yytext); return EQ; }
":="      { yylval = create_node(yytext); return ASSIGN; }
"<"       { yylval = create_node(yytext); return LESSTHAN; }
"("       { yylval = create_node(yytext); return LPAREN; }
")"       { yylval = create_node(yytext); return RPAREN; }
{number}  { yylval = create_node(yytext); return NUMBER; }
{varname} { yylval = create_node(yytext); return ID; }
.         { return yytext[0]; }
