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

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <vector>

#include "c2_koch.h"

using namespace std;
typedef unsigned int uint;

static void print_lines(vector < line > & lines, FILE * f)
{
	vector < line > ::iterator b = lines.begin(), e = lines.end();
	for (vector < line > ::iterator it = b; it != e; ++it)
	{
		line & l = *it;
		fprintf(f, "{%.16lf, %.16lf}-{%.16lf, %.16lf}\n", 
			l.p1.x, l.p1.y,
			l.p2.x, l.p2.y);
	}
}

static void koch(point p1, point p2, vector < pair < line, int > > & lst,
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

static void koch(vector < pair < line, int > > & lines, int itr, int maxItr)
{
	if (itr >= maxItr) return;

	vector < pair < line, int > > ::iterator b = lines.begin(), e = lines.end();
	vector < pair < line, int > > lines_new;

	lines_new.reserve(lines.size() * 4);

	for (vector < pair < line, int > > ::iterator it = b; it != e; ++it)
	{
		line & l = it->first;
		koch(l.p1, l.p2, lines_new, it->second);
	}

	lines.swap(lines_new);
	koch(lines, ++itr, maxItr);
}


void koch(vector < line > & ret, int maxItr, double r)
{
	vector < pair < line, int > > lines;
	point p1 = point(0, r);
	point p2 = point(r * cos(- M_PI / 6.0), r * sin(- M_PI / 6.0));

	lines.push_back(make_pair(line(p1, p2), 59));

	p1    = point(- r * cos(- M_PI / 6.0), r * sin(- M_PI / 6.0));
	p2    = point(0, r);
	lines.push_back(make_pair(line(p1, p2), 150));

	lines.push_back(make_pair(line(
	                          point(- r * cos(- M_PI / 6.0), r * sin(- M_PI / 6.0)),
	                          point(r * cos(- M_PI / 6.0), r * sin(- M_PI / 6.0))), 270));

	koch(lines, 0, maxItr);

	ret.reserve(lines.size());
	for (uint i = 0; i < lines.size(); ++i) {
		ret.push_back(lines[i].first);
	}
}

int main(int argc, char * argv[]) {
	double r   = 1.5;
	int maxItr = 5;

	if (argc > 1) {
		maxItr = atoi(argv[1]);
	}

	if (argc > 2) {
		double r1;
		if (sscanf(argv[2], "%lf", &r1) == 1) {
			r = r1;
		}
	}

	vector < line > lines;
	koch(lines, maxItr, r);

	FILE * f = fopen("output.txt", "w");
	print_lines(lines, f);
	fclose(f);
	return 0;
}

