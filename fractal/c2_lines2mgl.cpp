#include <stdio.h>
#include "c2_lsystem.h"
#include "c2_2graph.h"

using namespace std;

string print_lines2mgl(Group & g, list < line > & ln,
                       double min_x, double max_x, double min_y, double max_y)
{
	string fname = "output.mgl";

	if (!g.name.empty()) fname = g.name + ".mgl"; 
	FILE * f = fopen(fname.c_str(), "w");

	fprintf(f, "axis %lf %lf %lf %lf\n", min_x, min_y, max_x, max_y);
	for (list < string> :: iterator it = g.mgl.begin(); it != g.mgl.end(); ++it)
	{
		fprintf(f, "%s\n", it->c_str());
	}

	for (list < line >::iterator it = ln.begin(); it != ln.end(); ++it)
	{
		fprintf(f, "line %.16lf %.16lf %.16lf %.16lf\n",
		            it->x0, it->y0, it->x1, it->y1);
	}

	fclose(f);
	return fname;
}

