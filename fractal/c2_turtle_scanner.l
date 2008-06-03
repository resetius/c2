%{
struct State {
	int s;
	double m;
};

struct State st;

#define NUMBER 256

%}
%option 8bit
%option outfile="c2_turtle_scanner.c" header-file="c2_turtle_scanner.h" prefix="turtle"
%s mode
NUM [0-9]+\.?[0-9]*
VAR [a-zA-Z]
%%
<INITIAL>"F"   { /*forward*/ return yytext[0]; }
<INITIAL>"B"   { /*skip*/    return yytext[0]; }
<INITIAL>"!"   { /*inverse*/ return yytext[0]; }
<INITIAL>"@"   { /*modifier*/ BEGIN(mode); return yytext[0]; }
<mode>{NUM}    { /*len multiply*/ sscanf(yytext, "%lf", &st.m); BEGIN(INITIAL); return NUMBER; }
<INITIAL>{VAR} { /*replace*/ return yytext[0]; }
.|"\n"         {}
%%

