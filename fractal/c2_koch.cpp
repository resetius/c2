
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <list>

#include "c2_koch.h"

using namespace std;
typedef unsigned int uint;

static void print_lines(list < line > & lines, FILE * f)
{
	list < line > ::iterator b = lines.begin(), e = lines.end();
	for (list < line > ::iterator it = b; it != e; ++it)
	{
		fprintf(f, "{%.16lf, %.16lf}-{%.16lf, %.16lf}\n", 
			it->p1.x, it->p1.y,
			it->p2.x, it->p2.y);
	}
}

static void koch(point p1, point p2, list < line > & lst)
{
	point p3, p4, p5;
	double a;
	p3.x = (p1.x + p2.x) / 3.0;
	p3.y = (p1.y + p2.y) / 3.0;

	p4.x = 2.0 * (p1.x + p2.x) / 3.0;
	p4.y = 2.0 * (p1.y + p2.y) / 3.0;

	a = sqrt((p1.x - p3.x) * (p1.x - p3.x) + 
	         (p1.y - p3.y) * (p1.y - p3.y));
	p5.x = p3.x - a * cos(2.0 * M_PI / 3.0);
	p5.y = p3.y + a * sin(2.0 * M_PI / 3.0);

	lst.push_back(line(p1, p3));
	lst.push_back(line(p4, p2));
	lst.push_back(line(p3, p5));
	lst.push_back(line(p5, p4));
}

void koch(list < line > & lines, int itr, int maxItr)
{
	if (itr >= maxItr) return;

	list < line > ::iterator b = lines.begin(), e = lines.end();
	list < line > lines_new;

	for (list < line > ::iterator it = b; it != e; ++it)
	{
		koch(it->p1, it->p2, lines_new);
	}

	lines.swap(lines_new);
	koch(lines, ++itr, maxItr);
}

int main() {
	list < line > lines;
	lines.push_back(line(point(0, 0), point(1, 0)));
	koch(lines, 0, 1);

	FILE * f = fopen("output.txt", "w");
	print_lines(lines, f);
	fclose(f);
	return 0;
}
