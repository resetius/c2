%{
#include <math.h>
#include "c2_lsystem_parser.hpp"

static int comment_caller;

%}
%option 8bit 
%option outfile="c2_lsystem_scanner.c" header-file="c2_lsystem_scanner.h"
%s gen
%s grp
%s comment
AXIOM axiom
ALPHA alpha
ANGLE angle
THETA theta
PI pi
NUM [0-9]+\.?[0-9]*
VAR [a-zA-Z]
LS  [a-zA-Z\+\-\[\]!0-9]+
IDT [a-zA-Z\.0-9]+
OP  [=/\*\+\-\(\)]
%%
";"   { comment_caller = YY_START; BEGIN(comment); }
<comment>"\n" { BEGIN(comment_caller); if (comment_caller != INITIAL) return '\n'; }

<INITIAL>{IDT}  { printf("grp idt %s\n", yytext); yylval.str = strdup(yytext); return GRPIDT; }
<grp>{NUM}   { sscanf(yytext, "%lf", &yylval.num); return NUM; }
<gen>{LS}    { printf("ls=%s\n", yytext); yylval.str = strdup(yytext); BEGIN(grp); return TURTLE; }
"{"          { printf("grp\n"); BEGIN(grp);     return yytext[0]; }
"}"          { printf("endof grp\n"); BEGIN(INITIAL); return yytext[0]; }
<grp>{AXIOM} { printf("axiom\n"); BEGIN(gen); return AXIOM; }
<grp>{ALPHA} { printf("alpha\n"); return ALPHA; }
<grp>{THETA} { printf("alpha\n"); return THETA; }
<grp>{ANGLE} { return ANGLE; }
<grp>{VAR}   { printf("var=%s\n", yytext); yylval.str = strdup(yytext); BEGIN(gen); return VAR; }
<grp>{PI}    { yylval.num = M_PI; return NUM; }
<grp>{OP}    { printf("op=%s\n", yytext); return yytext[0]; }
"="          { printf("op=%s\n", yytext); return yytext[0]; }
<grp,gen>"\n" { return yytext[0]; }
.|"\n"       {}
%%
/*
 подсказка:
 без указания группы действует во всех группах
<INITIAL> действует в первоначальной группе.
 */

