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
#include <string.h>
#include <ctype.h>
#include "c2_lsystem_parser.hpp"

static int comment_caller;

static char * strdup_upper(const char * p)
{
    int l = strlen(p);
    char * r = (char*)malloc(l + 1);
    char *r1 = r; 
    while ((*r++ = toupper(*p++)));
    return r1;
}

%}
%option 8bit 
%option header-file="c2_lsystem_scanner.h" case-insensitive
%s gen
%s grp
%s comment
%s mgl
%s order
AXIOM axiom
ALPHA alpha
ANGLE angle
THETA theta
PI pi
NUM [0-9]+\.?[0-9]*
VAR [a-zA-Z]
LS  [a-zA-Z\+\-\[\]!0-9@\.\|\<\>\\/:]+
IDT [a-zA-Z\.0-9&\*_@!\+\-]+
OP  [=/\*\+\-\(\)]
COMMENT ;.*\n?
WHITESPACE [ \t]
%%

<INITIAL,gen,grp>";" {
     /*  printf("begin comment %d\n", YY_START);*/
       comment_caller = YY_START;
       BEGIN(comment);
    }

<comment>[ \t]*"mgl"[\ t]+ {
                BEGIN(mgl);
                return MGL;
             }
<comment>[ \t]*"order"[\ t]+ {
                if (comment_caller != INITIAL) {
                    BEGIN(order);
                    return ORDER;
                }
             }
<mgl>.* {
 /*             printf("mgl: %s\n", yytext);*/
             yylval.str = strdup(yytext);
             BEGIN(comment);
             return MGLSTR;
        }

<order>[0-9]+ {
/*                  printf("order: %s\n", yytext);*/
                  sscanf(yytext, "%lf", &yylval.num);
                  BEGIN(comment);
                  return NUM;
              }

<comment,mgl>"\n" { 
/*             printf("end of comment %d\n", comment_caller);*/
             BEGIN(comment_caller); 
             ++yylineno;
             if (comment_caller != INITIAL) {
                  return '\n'; 
             }
         }

<INITIAL>{IDT}  { 
                /* printf("grp idt =  %s\n", yytext);  */
                    yylval.str = strdup(yytext); 
                    return GRPIDT; 
                }
<grp>{NUM}   { sscanf(yytext, "%lf", &yylval.num);
               /* printf("number %lf\n", yylval.num);  */
               return NUM; 
             }
<gen>{LS}    { /* printf("ls=%s\n", yytext);  */
               yylval.str = strdup_upper(yytext); 
               BEGIN(grp); return TURTLE; }
<INITIAL,grp,gen>"{" { /* printf("grp\n"); */ 
                       BEGIN(grp);     return yytext[0]; }
<grp,gen>"}" { /* printf("endof grp\n"); */ 
                  BEGIN(INITIAL); return yytext[0]; }
<grp>{AXIOM} { /* printf("axiom\n"); */ 
                  BEGIN(gen); return AXIOM; }
<grp>{ALPHA} { /* printf("alpha\n"); */ return ALPHA; }
<grp>{THETA} { /* printf("theta\n"); */ return THETA; }
<grp>{ANGLE} { /* printf("angle\n"); */ return ANGLE; }
<grp>{VAR}   { 
               /* printf("var=%s\n", yytext); */
               yylval.str = strdup_upper(yytext);
               BEGIN(gen); return VAR; 
             }
<grp>{PI}    { yylval.num = M_PI; return NUM; }
<grp>{OP}    { /* printf("op=%s\n", yytext); */ return yytext[0]; }
<grp,gen>"=" { /* printf("op=%s\n", yytext); */ return yytext[0]; }
<grp,gen>"\n" { BEGIN(grp); ++yylineno; return '\n'; }
.       {}
"\n"    { ++yylineno; }
%%
/*
 подсказка:
 без указания группы действует во всех группах
<INITIAL> действует в первоначальной группе.
 */

int yywrap()
{
	return 1;
}

