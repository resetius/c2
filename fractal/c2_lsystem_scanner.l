%{
#include "c2_lsystem_parser.hpp"
%}
%option 8bit 
%option outfile="c2_lsystem_scanner.c" header-file="c2_lsystem_scanner.h"
AXIOM axiom
ALPHA alpha
THETA theta
NUM [0-9]+\.?[0-9]+
VAR [a-zA-Z\[\]\+\-]
%%
{NUM}   { sscanf(yytext, "%lf", &yylval.num); printf("'%s'='%lf'\n", yytext, yylval.num); return NUM; }
{VAR}   { yylval.str = strdup(yytext); printf("'%s'\n", yytext); return VAR; }
{AXIOM} { printf("axiom '%s'\n", yytext); return AXIOM; }
{ALPHA} { printf("alpha '%s'\n", yytext); return ALPHA; }
{THETA} { printf("theha '%s'\n", yytext); return THETA; }
"="|";" { return yytext[0]; }
.       { /* printf("other: '%s'\n", yytext); */ }
%%

