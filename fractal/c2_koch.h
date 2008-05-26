#ifndef C2_KOCH_H
#define C2_KOCH_H

#include <vector>

struct point {
	double x;
	double y;

	point () : x(0), y(0) {}
	point (double x1, double y1) : x(x1), y(y1) {}
};

struct line {
	point p1;
	point p2;

	line() {};
	line (point p1_, point p2_) : p1(p1_), p2(p2_) {}
};

void koch(std::vector < line > & lines, int maxItr, double r);

#endif /* C2_KOCH_H */
