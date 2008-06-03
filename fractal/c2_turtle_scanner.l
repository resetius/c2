%{
#include <math.h>
#include "c2_turtle.h"
%}
%option 8bit
%option outfile="c2_turtle_scanner.c" header-file="c2_turtle_scanner.h" prefix="turtle"
%s len
NUM [0-9]+\.?[0-9]*
INT [0-9]+
VAR [a-zA-Z]
%%
<INITIAL>"F"   { /*forward*/ return yytext[0]; }
<INITIAL>"B"   { /*skip*/    return yytext[0]; }
<INITIAL>"!"   { /*inverse*/ return yytext[0]; }
<INITIAL>"C"{INT} { /*color*/
                 sscanf(&yytext[1],"%d", &st.i); 
                 st.str = strdup(yytext);
                 return COLOR; 
               }
<INITIAL>"<1"  { /*next color*/
               }
<INITIAL>"@"   { /*line len modifier*/ BEGIN(len); return yytext[0]; }
<len>{NUM}     { /*len multiply*/ sscanf(yytext,   "%lf", &st.m); st.str = strdup(yytext); BEGIN(INITIAL); return NUMBER; }
<len>"g"{NUM}  { /*square root*/ sscanf(&yytext[1],"%lf", &st.m); 
                 st.m   = sqrt(st.m);
                 st.str = strdup(yytext); 
                 BEGIN(INITIAL);
                 return NUMBER; 
               }
<len>"I"{NUM} { /*inverse*/ sscanf(&yytext[1],"%lf", &st.m); 
                 st.m   = 1.0 / sqrt(st.m);
                 st.str = strdup(yytext);  
                 BEGIN(INITIAL);
                 return NUMBER;
               }
<INITIAL>{VAR} { /*replace*/ return yytext[0]; }
.|"\n"         {}
%%

