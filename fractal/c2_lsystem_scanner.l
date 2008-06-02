%{
#include "c2_lsystem_parser.hpp"
%}
%option 8bit 
%option outfile="c2_lsystem_scanner.c" header-file="c2_lsystem_scanner.h"
AXIOM axiom
ALPHA alpha
THETA theta
NUM [0-9]+\.?[0-9]+
VAR [a-zA-Z\+\-]
OP [=\|;\[\]\{\}]
%%
{NUM}   { sscanf(yytext, "%lf", &yylval.num); return NUM; }
{VAR}   { yylval.str = strdup(yytext); return VAR; }
{AXIOM} { return AXIOM; }
{ALPHA} { return ALPHA; }
{THETA} { return THETA; }
{OP}    { return yytext[0]; }
"\n"    { return yytext[0]; }
.       { }
%%
