#include <stdio.h>
#include <math.h>
#include <list>
extern "C" {
#include "c2_lsystem_scanner.h"
}
#include "c2_lsystem_parser.hpp"
#include "c2_2graph.h"
#include "c2_lsystem.h"

using namespace std;

void normalize(list < line > & ln, double & mnx, double & mxx, double & mny, double &mxy) {
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
	double kk = std::min(xx, yy);

	double x = ln.begin()->x0, y = ln.begin()->y0;
	mnx = mxx = (x - min_x) * yy - 1.0;
	mny = mxy = (y - min_y) * yy - 1.0;

	for (list < line >::iterator it = ln.begin(); it != ln.end(); ++it)
	{
		it->x0 = (it->x0 - min_x) * kk - 1.0;
		it->x1 = (it->x1 - min_x) * kk - 1.0;

		it->y0 = (it->y0 - min_y) * kk - 1.0;
		it->y1 = (it->y1 - min_y) * kk - 1.0;

		if (mxx < it->x0) mxx = it->x0;
		if (mxx < it->x1) mxx = it->x1;
		if (mnx > it->x0) mnx = it->x0;
		if (mnx > it->x1) mnx = it->x1;

		if (mxy < it->y0) mxy = it->y0;
		if (mxy < it->y1) mxy = it->y1;
		if (mny > it->y0) mny = it->y0;
		if (mny > it->y1) mny = it->y1;
	}
}

string print_lines(Group & g, list < line > & ln, 
		double min_x, double max_x, double min_y, double max_y, bool mgl)
{
	string fname = "output.mgl";
	
	if (!g.name.empty()) fname = g.name;
       
	fname = (mgl) ? fname + ".mgl" : fname + ".txt";

	FILE * f = fopen(fname.c_str(), "w");

	if (mgl) {
	fprintf(f, "axis %lf %lf %lf %lf\n", min_x, min_y, max_x, max_y);
	for (list < string> :: iterator it = g.mgl.begin(); it != g.mgl.end(); ++it)
	{
		fprintf(f, "%s\n", it->c_str());
	}
	}

	for (list < line >::iterator it = ln.begin(); it != ln.end(); ++it)
	{
		if (!mgl) {
		fprintf(f, "{%.16lf, %.16lf}-{%.16lf, %.16lf} %d\n",
				it->x0, it->y0, it->x1, it->y1, it->c);
		} else { /*mgl*/
		fprintf(f, "line %.16lf %.16lf %.16lf %.16lf\n",
				it->x0, it->y0, it->x1, it->y1);
		}
	}
	fclose(f);

	return fname;
}

void usage(const char * n)
{
	printf("usage:\n");
	printf("%s [-f filename] [-t type] [-l order] \n", n);
	printf("-f -- read lsystem from file (stdin - default)\n");
	printf("-t -- points save type: \n");
	printf("\t\t mgl -- MathGL script\n");
	printf("\t\t png -- PNG\n");
	printf("\t\t default -- txt (for vizualizer)\n");
	printf("-l -- default order \n");
	exit(0);
}

int main(int argc, char * argv[])
{
	Parser p;
	FILE * f  = 0;
	int level = 0;
	int type  = 0; //txt

	for (uint i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-f") && i < argc - 1) {
			f = fopen(argv[i + 1], "r");
		} else if (!strcmp(argv[i], "-l") && i < argc - 1) {
			level = atoi(argv[i + 1]);
		} else if (!strcmp(argv[i], "-t") && i < argc - 1) {
			if (!strcmp(argv[i + 1], "mgl")) {
				type = 1;
			} else if (!strcmp(argv[i + 1], "png")) {
				type = 2;
			}			
		} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			usage(argv[0]);
		}
	}

	if (f) yyrestart(f);

	try {
		while (yyparse(&p));
	} catch (std::exception & e) {
		cerr << "error: " << e.what() << "\n";
		return -1;
	}
	
	p.print();

	for (list < Group > ::iterator it = p.grp.begin(); it != p.grp.end(); ++it)
	{
		if (!it->check()) continue;

		int l = level;
		if (it->order != 0 /*&& level == 0*/) l = it->order;

		cerr << "building " << it->name << "\n";
		cerr << "  using order " << l << "\n";

		try {
			double min_x, max_x, min_y, max_y;
			string W = lsystem(*it, l);
			cerr << "  lsystem done\n";
			list < line > lines = turtle(*it, W);
			cerr << "  turtle done\n";
			normalize(lines, min_x, max_x, min_y, max_y);
			cerr << "  normilize done\n";
			switch (type) {
			case 0:
				print_lines2txt(*it, lines, min_x, max_x, min_y, max_y);
				break;
			case 1:
				print_lines2mgl(*it, lines, min_x, max_x, min_y, max_y);
				break;
			case 2:
				print_lines2png(*it, lines, min_x, max_x, min_y, max_y);
				break;
			}
			cerr << "  saving done\n";
		} catch (std::exception & e) {
			cerr << "error: " << e.what() << "\n";
			continue;
		} catch (...) {
			cerr << "error 2\n";
			exit(1);
		}
	}

	if (f) fclose(f);
}

