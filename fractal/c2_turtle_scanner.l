%{
#include "c2_turtle.h"
%}
%option 8bit
%option outfile="c2_turtle_scanner.c" header-file="c2_turtle_scanner.h" prefix="turtle"
%s mode
NUM [0-9]+\.?[0-9]*
VAR [a-zA-Z]
%%
<INITIAL>"F"   { /*forward*/ st.s = 0; return yytext[0]; }
<INITIAL>"B"   { /*skip*/    st.s = 0; return yytext[0]; }
<INITIAL>"!"   { /*inverse*/ st.s = 0; return yytext[0]; }
<INITIAL>"@"   { /*modifier*/ BEGIN(mode); st.s = 1; return yytext[0]; }
<mode>{NUM}    { /*len multiply*/ sscanf(yytext, "%lf", &st.m); st.str = strdup(yytext); BEGIN(INITIAL); st.s = 0; return NUMBER; }
<INITIAL>{VAR} { /*replace*/ return yytext[0]; }
.|"\n"         {}
%%

