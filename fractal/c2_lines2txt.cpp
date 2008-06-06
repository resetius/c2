#include <stdio.h>
#include "c2_lsystem.h"
#include "c2_2graph.h"

using namespace std;

string print_lines2txt(Group & g, list < line > & ln,
                       double min_x, double max_x, double min_y, double max_y)
{
	string fname = "output.txt";

	if (!g.name.empty()) fname = g.name + ".txt";
	FILE * f = fopen(fname.c_str(), "w");

	for (list < line >::iterator it = ln.begin(); it != ln.end(); ++it)
	{
		fprintf(f, "{%.16lf, %.16lf}-{%.16lf, %.16lf} %d\n",
		            it->x0, it->y0, it->x1, it->y1, it->c);
	}

	return fname;
}

