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

#include <stdio.h>
#include <math.h>
#include <list>
#include "c2_lsystem_scanner.h"
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
//	printf("%s [-f filename] [-t type] [-l order] \n", n);
	printf("%s [-f filename] [-l order] \n", n);
	printf("-f -- read lsystem from file (stdin - default)\n");
#if 0
	printf("-t -- points save type: \n");
	printf("\t\t mgl -- MathGL script\n");
	printf("\t\t png -- PNG\n");
	printf("\t\t default -- txt (for vizualizer)\n");
#endif
	printf("-l -- default order \n");
	exit(0);
}

int main(int argc, char * argv[])
{
	Parser p;
	FILE * f  = 0;
	int level = 0;
	int type  = 0; //txt

	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-f") && i < argc - 1) {
			f = fopen(argv[i + 1], "r");
		} else if (!strcmp(argv[i], "-l") && i < argc - 1) {
			level = atoi(argv[i + 1]);
#if 0
		} else if (!strcmp(argv[i], "-t") && i < argc - 1) {
			if (!strcmp(argv[i + 1], "mgl")) {
				type = 1;
			} else if (!strcmp(argv[i + 1], "png")) {
				type = 2;
			}
#endif
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
#if 0
			switch (type) {
			case 0:
				print_lines2txt(*it, lines, min_x, max_x, min_y, max_y);
				break;
			case 1:
				print_lines2mgl(*it, lines, min_x, max_x, min_y, max_y);
				break;
			case 2:
#endif
			{
				print_lines2png(*it, lines, min_x, max_x, min_y, max_y);
#if 0
				break;
#endif
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

	return 0;
}

