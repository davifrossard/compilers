%option outfile="scanner.c"
%option noyywrap
%option nounput
%option noinput
%{
#include "parser.h"
%}
number [0-9]+\.?|[0-9]*\.[0-9]+
hex 0x[0-9A-Fa-f]+
neg \-
%%
[ ]      ; // skip blanks
{number} { yylval = atof(yytext); return NUMBER; }
{hex} 	 { yylval = strtol(yytext, NULL, 16); return HEX; }
if 	 { return IF; }
else	 { return ELSE; }
==	 { return EQ; }	 
\n|.     { return yytext[0]; }
