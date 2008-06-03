#include <stdio.h>
#include <math.h>
#include <list>
extern "C" {
#include "c2_lsystem_scanner.h"
}
#include "c2_lsystem_parser.hpp"
#include "c2_lsystem.h"

using namespace std;

void normalize(list < line > & ln) {
	double max_x, min_x;
	double max_y, min_y;
	max_x = min_x = ln.begin()->x0;
	max_y = min_y = ln.begin()->y0;

	for (list < line >::iterator it = ln.begin(); it != ln.end(); ++it)
	{
		if (max_x < it->x0) max_x = it->x0;
		if (max_x < it->x1) max_x = it->x1;
		if (min_x > it->x0) min_x = it->x0;
		if (min_x > it->x1) min_x = it->x1;

		if (max_y < it->y0) max_y = it->y0;
		if (max_y < it->y1) max_y = it->y1;
		if (min_y > it->y0) min_y = it->y0;
		if (min_y > it->y1) min_y = it->y1;
	}

	double xx = 2.0 / (max_x - min_x);
	double yy = 2.0 / (max_y - min_y);

	for (list < line >::iterator it = ln.begin(); it != ln.end(); ++it)
	{
		it->x0 = (it->x0 - min_x) * yy - 1.0;
		it->x1 = (it->x1 - min_x) * yy - 1.0;

		it->y0 = (it->y0 - min_y) * yy - 1.0;
		it->y1 = (it->y1 - min_y) * yy - 1.0;
	}
}

void print_lines(Group & g, list < line > & ln) {
	string fname = "output.txt";
	if (!g.name.empty()) fname = g.name + ".txt";
	FILE * f = fopen(fname.c_str(), "w");
	for (list < line >::iterator it = ln.begin(); it != ln.end(); ++it)
	{
		fprintf(f, "{%.16lf, %.16lf}-{%.16lf, %.16lf}\n",
				it->x0, it->y0, it->x1, it->y1);
	}
	fclose(f);
}

int main(int argc, char * argv[])
{
	Parser p;
	FILE * f  = 0;
	int level = 0;

	if (argc > 1) {
		f = fopen(argv[1], "r");
	}

	if (argc > 2) {
		level = atoi(argv[2]);
	}

	if (!level) level = 1;
	if (f) yyrestart(f);
	
	while (yyparse(&p));
	p.print();

	for (list < Group > ::iterator it = p.grp.begin(); it != p.grp.end(); ++it)
	{
		if (!it->check()) continue;

		try {
			string W = lsystem(*it, level);
			list < line > lines = turtle(*it, W);
			normalize(lines);
			print_lines(*it, lines);
		} catch (...) {
			cerr << "error\n";
			continue;
		}
	}

	if (f) fclose(f);
}

