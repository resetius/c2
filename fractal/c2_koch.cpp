
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <list>

#include "c2_koch.h"

using namespace std;
typedef unsigned int uint;

static void print_lines(list < pair < line, int > > & lines, FILE * f)
{
	list < pair < line, int > > ::iterator b = lines.begin(), e = lines.end();
	for (list < pair < line, int > > ::iterator it = b; it != e; ++it)
	{
		line & l = it->first;
		fprintf(f, "{%.16lf, %.16lf}-{%.16lf, %.16lf}\n", 
			l.p1.x, l.p1.y,
			l.p2.x, l.p2.y);
	}
}

static void koch(point p1, point p2, list < pair < line, int > > & lst,
				 int angle)
{
	point p3, p4, p5;
	double a, l, s;

	p3.x = p1.x +       (p2.x - p1.x) / 3.0;
	p4.x = p1.x + 2.0 * (p2.x - p1.x) / 3.0;

	p3.y = p1.y +       (p2.y - p1.y) / 3.0;
	p4.y = p1.y + 2.0 * (p2.y - p1.y) / 3.0;

	a = sqrt((p1.x - p3.x) * (p1.x - p3.x) + 
	         (p1.y - p3.y) * (p1.y - p3.y));

	s    = (angle > 180) ? -1 : 1;
	p5.x = - s * (p2.y - p1.y) / (p2.x - p1.x);
	p5.y = + s * 1.0;

	l = sqrt(p5.x * p5.x + 1.0);

	p5.x = p1.x + 0.5 * (p2.x - p1.x) + a * p5.x / l;

	p5.y = p1.y + 0.5 * (p2.y - p1.y) + a * p5.y / l;

	lst.push_back(make_pair(line(p1, p3), angle));
	lst.push_back(make_pair(line(p4, p2), angle));
	lst.push_back(make_pair(line(p3, p5), (360 + angle + 60) % 360));
	lst.push_back(make_pair(line(p5, p4), (360 + angle - 60) % 360));
}

void koch(list < pair < line, int > > & lines, int itr, int maxItr)
{
	if (itr >= maxItr) return;

	list < pair < line, int > > ::iterator b = lines.begin(), e = lines.end();
	list < pair < line, int > > lines_new;

	for (list < pair < line, int > > ::iterator it = b; it != e; ++it)
	{
		line & l = it->first;
		koch(l.p1, l.p2, lines_new, it->second);
	}

	lines.swap(lines_new);
	koch(lines, ++itr, maxItr);
}

int main() {
	list < pair < line, int > > lines;

	double r = 1.5;
	point p1 = point(0, r);
	point p2 = point(r * cos(- M_PI / 6.0), r * sin(- M_PI / 6.0));
	lines.push_back(make_pair(line(p1, p2), 59));

	p1    = point(- r * cos(- M_PI / 6.0), r * sin(- M_PI / 6.0));
	p2    = point(0, r);
	lines.push_back(make_pair(line(p1, p2), 150));

	lines.push_back(make_pair(line(
		point(- r * cos(- M_PI / 6.0), r * sin(- M_PI / 6.0)),
		point(r * cos(- M_PI / 6.0), r * sin(- M_PI / 6.0))), 270));

	koch(lines, 0, 6);

	FILE * f = fopen("output.txt", "w");
	print_lines(lines, f);
	fclose(f);
	return 0;
}
