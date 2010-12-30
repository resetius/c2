%{
/*$Id$*/

/* Copyright (c) 2008 Alexey Ozeritsky (Алексей Озерицкий)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Alexey Ozeritsky.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include <math.h>
#include "c2_turtle.h"

extern "C" int isatty(int) throw ();

struct State st = {0, 0, 0};
%}
%option 8bit
%option header-file="c2_turtle_scanner.h" prefix="turtle" case-insensitive
NUM [0-9]*\.?[0-9]+
INT [0-9]+
VAR [a-zA-Z]
%%
"f"|"d"   { /*forward*/ return 'F'; }
"g"|"m"   { /*skip*/    return 'G'; }
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
              sscanf(&yytext[1],"%d", &st.i);
              st.str = strdup(yytext);
              return INCCOLOR;
          }
">"{INT}  { /*prev color */
              sscanf(&yytext[1],"%d", &st.i);
              st.i   = -st.i;
              st.str = strdup(yytext);
              return INCCOLOR;
          }
"|"       { /*inc 180 deg*/
                 st.i   = 180;
                 st.str = strdup("|");
                 return INCNUMBER;
          }
"\\"{INT} { /*inc INT deg */
                 sscanf(&yytext[1], "%d", &st.i);
                 st.str = strdup(yytext);
                 return INCNUMBER;
          }
"/"{INT}  { /*dec INT deg */
                 sscanf(&yytext[1], "%d", &st.i);
                 st.i   = -st.i;
                 st.str = strdup(yytext);
                 return INCNUMBER;
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
                 st.m   = 1.0 / st.m;
                 st.str = strdup(yytext);  
                 BEGIN(INITIAL);
                 return NUMBER;
          }
"@IQ"{NUM} { /*inverse*/ sscanf(&yytext[3],"%lf", &st.m); 
                 st.m   = 1.0 / sqrt(st.m);
                 st.str = strdup(yytext);  
                 BEGIN(INITIAL);
                 return NUMBER;
           }
"@QI"{NUM} { /*inverse*/ sscanf(&yytext[3],"%lf", &st.m); 
                 st.m   = 1.0 / sqrt(st.m);
                 st.str = strdup(yytext);  
                 BEGIN(INITIAL);
                 return NUMBER;
           }
{VAR} { /*replace*/ return yytext[0]; }
.|"\n"    {}
%%
/*
  The standard drawing commands are:
      F Draw forward
      G Move forward (without drawing)
      + Increase angle
      - Decrease angle
      | Try to turn 180 degrees. (If angle is odd, the turn
        will be the largest possible turn less than 180 degrees.)

 These commands increment angle by the user specified angle value. They should be used when possible because they are fast. If greater flexibility is needed, use the following commands which keep a completely separate angle pointer which is specified in degrees. 
      D   Draw forward
      M   Move forward
      \nn Increase angle nn degrees
      /nn Decrease angle nn degrees

  Color control:
      Cnn Select color nn
      < nn Increment color by nn
      > nn decrement color by nn

  Advanced commands:
      !     Reverse directions (Switch meanings of +, - and , /)
      @nnn  Multiply line segment size by nnn
            nnn may be a plain number, or may be preceded by
                I for inverse, or Q for square root.
                (e.g.  @IQ2 divides size by the square root of 2)
      [     Push.  Stores current angle and position on a stack
      ]     Pop.  Return to location of last push
*/
