%{
#include <math.h>
#include "c2_turtle.h"

struct State st = {0, 0, 0};
%}
%option 8bit
%option outfile="c2_turtle_scanner.c" header-file="c2_turtle_scanner.h" prefix="turtle" case-insensitive
NUM [0-9]*\.?[0-9]+
INT [0-9]+
VAR [a-zA-Z]
%%
"f"   { /*forward*/ return 'F'; }
"g"   { /*skip*/    return 'G'; }
"!"       { /*inverse*/ return '!'; }
"+"       { /*inc angle*/ return '+'; }
"-"       { /*dec angle*/ return '-'; }
"["|"]"   { /*branch */   return yytext[0]; }
"C"{INT}  { /*color*/
                 sscanf(&yytext[1],"%d", &st.i); 
                 st.str = strdup(yytext);
                 return COLOR; 
          }
"<"{INT}  { /*next color*/
          }
">"{INT}  { /*prev color */
          }
"@"{NUM}  { /*len multiply*/ sscanf(&yytext[1], "%lf", &st.m); 
              st.str = strdup(yytext); return NUMBER; }
"@q"{NUM} { /*square root*/ sscanf(&yytext[2],"%lf", &st.m); 
                 st.m   = sqrt(st.m);
                 st.str = strdup(yytext); 
                 BEGIN(INITIAL);
                 return NUMBER; 
          }
"@I"{NUM} { /*inverse*/ sscanf(&yytext[2],"%lf", &st.m); 
                 st.m   = 1.0 / sqrt(st.m);
                 st.str = strdup(yytext);  
                 BEGIN(INITIAL);
                 return NUMBER;
          }
{VAR} { /*replace*/ return yytext[0]; }
.|"\n"    {}
%%
