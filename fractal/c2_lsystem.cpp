#include <stdio.h>
extern "C" {
#include "c2_lsystem_scanner.h"
}
#include "c2_lsystem_parser.hpp"
#include "c2_lsystem.h"

int main(int argc, char * argv[])
{
	Parser p;
	FILE * f = 0;

	if (argc > 1) {
		f = fopen(argv[1], "r");
	}

	if (f) yyrestart(f);
	
	while (yyparse(&p));
	p.check();
	p.print();

	if (f) fclose(f);
}

