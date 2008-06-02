#include <stdio.h>
#include <math.h>
#include <stack>
#include <list>
extern "C" {
#include "c2_lsystem_scanner.h"
}
#include "c2_lsystem_parser.hpp"
#include "c2_lsystem.h"

using namespace std;

string lsystem(Parser & p, int level) {
	string W = p.axiom;
	for (int i = 0; i < level; ++i) {
		string nW;
		for (uint j = 0; j < W.length(); ++j) {
			if (W[j] == '-') nW += "-";
			else if (W[j] == '+') nW += "+";
			else if (W[j] == '[') nW += "[";
			else if (W[j] == ']') nW += "]";
			else {
				nW += p.r[W[j]];
			}
		}
		W.swap(nW);
	}
	return W;
}

struct Context {
	double a;
	double x0;
	double y0;

	Context(double alpha, double x, double y)
		: a(alpha), x0(x), y0(y) {}
};

struct line {
	double x0;
	double y0;
	double x1;
	double y1;
};

list < line > turtle(Parser & p, const string & W)
{
	stack < Context > st;
	list < line > ret;
	Context c (p.alpha, 0, 0);

	for (uint j = 0; j < W.length(); ++j) {
		if (W[j] == '+') c.a += p.theta;
		else if (W[j] == '-') c.a -= p.theta;
		else if (W[j] == 'B') {
			//пропуск
			c.x0 += cos(c.a); c.y0 += sin(c.a);
		} else if (W[j] == '[') {
			st.push(c);
		} else if (W[j] == ']') {
			c = st.top();
			st.pop();
		} else if (W[j] == 'F') {
			line l;
			l.x0 = c.x0; l.y0 = c.y0;
			c.x0 += cos(c.a); c.y0 += sin(c.a);
			l.x1 = c.x0; l.y1 = c.y0;
			ret.push_back(l);
		}
	}
	return ret;
}

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
		it->x0 = (it->x0 - min_x) * xx - 1.0;
		it->x1 = (it->x1 - min_x) * xx - 1.0;

		it->y0 = (it->y0 - min_y) * yy - 1.0;
		it->y1 = (it->y1 - min_y) * yy - 1.0;
	}
}

void print_lines(list < line > & ln) {
	FILE * f = fopen("output.txt", "w");
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
	p.check();
	p.print();

	if (!p.error) {
		string W = lsystem(p, level);
		list < line > lines = turtle(p, W);
		normalize(lines);
		print_lines(lines);
	}

	if (f) fclose(f);
}

