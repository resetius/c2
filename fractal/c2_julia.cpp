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
#include <string.h>
#include <stdlib.h>
#include <gd.h>

#include <complex>
#include <iostream>
#include <vector>
#include <set>
#include <map>

#include "common/c2_colormap_vga1.h"

using namespace std;
typedef complex < double > cmpl;

static void init_color_map(int *colors, gdImagePtr & im)
{
	for (uint i = 0; i < 256; ++i) {
		colors[i] = gdImageColorAllocate(im,
										 colormap_vga2[i][0],
										 colormap_vga2[i][1],
										 colormap_vga2[i][2]);
	}
}


struct LexicalCmp {
	template < typename A, typename B >
	bool operator () (const std::pair < A, B > & p1, const std::pair < A, B > & p2) const {
		if (p1.first < p2.first) {
			return true;
		} else if (p1.first == p2.first) {
			return p1.second < p2.second;
		} else {
			return false;
		}
	}
};

class CmplVec {
	vector < cmpl > d;
	friend CmplVec sqrt(const CmplVec & v);

public:
	CmplVec() {}

	CmplVec(const cmpl & v): d(1) {
		d[0] = v;
	}

	CmplVec(const vector < cmpl > & v): d(v) {}

	CmplVec(const CmplVec & v): d(v.d) {}
	CmplVec(const CmplVec & v1,
			const CmplVec & v2) {
		uint sz = v1.d.size() + v2.d.size();
		d.reserve(sz);
		for (uint i = 0; i < v1.d.size(); ++i) {
			d.push_back(v1.d[i]);
		}
		for (uint i = 0; i < v2.d.size(); ++i) {
			d.push_back(v2.d[i]);
		}
	}

	CmplVec operator - () const {
		CmplVec ret(*this);
		for (uint i = 0; i < ret.d.size(); ++i) {
			ret.d[i] = -ret.d[i];
		}
		return ret;
	}

	CmplVec operator - (const cmpl & val) const {
		CmplVec ret(*this);
		for (uint i = 0; i < ret.d.size(); ++i) {
			ret.d[i] -= val;
		}
		return ret;
	}

	CmplVec & operator = (const CmplVec & v) {
		d = v.d;
		return * this;
	}

	cmpl & operator [] (size_t i) {
		return d[i];
	}

	size_t size() const {
		return d.size();
	}

	void draw(gdImagePtr & im, int color, int h,
			  double a, double b, double xx, double yy) {
		for (uint i = 0; i < d.size(); ++i) {
			int x = (int)((d[i].real() - a) * xx);
			int y = (int)((d[i].imag() - b) * yy);
			gdImageSetPixel(im, x, (h - y), color);
		}
	}

	void clear() {
		d.clear();
	}

	void push_back(const cmpl & c) {
		d.push_back(c);
	}
};

CmplVec sqrt(const CmplVec & v)
{
	CmplVec ret(v);
	for (size_t i = 0; i < v.size(); ++i) {
		ret.d[i] = std::sqrt(ret.d[i]);
	}
	return ret;
}

/*с помощью обратной итерации*/
void julia2(int s, double c1, double c2)
{
	cmpl c(c1, c2);
	double a = - std::max(2.0, 2 * sqrt(std::abs(c)));
	double b = a;
	double l = -2.0 * a;

	int w = s;
	int h = s;
	int level = 20; //max iterations


	gdImagePtr im = gdImageCreate(w, h);
	int black = gdImageColorAllocate(im, 0, 0, 0);
	int white = gdImageColorAllocate(im, 255, 255, 255);
	gdImageFill(im, w - 1, h - 1, white);
	double xx = (double)w / l;
	double yy = (double)h / l;

	//part1: fixed point
	cmpl w1 = std::sqrt(1.0 - 4.0 * c);
	cmpl z1 = 0.5 * (1.0 + w1);
	cmpl z2 = 0.5 * (1.0 + w1);
	cmpl z  = (std::abs(z1) > std::abs(z2)) ? z1 : z2;

	//part2: iterations

	CmplVec W (std::sqrt(z - c));
	CmplVec Z (W, -W);

	Z.draw(im, black, h, a, b, xx, yy);

	for (uint k = 0; k < level; ++k) {
		W = sqrt(Z - c);
		Z = CmplVec(W, -W);
		Z.draw(im, black, h, a, b, xx, yy);
	}

	FILE * f = fopen("julia.png", "wb");
	gdImagePng(im, f);
	fclose(f);
	gdImageDestroy(im);
}

/*filled julia set*/
void julia(int s, double c1, double c2)
{
	// [-2, 2]
	// [-2, 2]
	// 4x4

	double a = -2.0;
	double b = -2.0;
	int w = s;
	int h = s;
	gdImagePtr im = gdImageCreate(w, h);
	int black = gdImageColorAllocate(im, 0, 0, 0);
	int white = gdImageColorAllocate(im, 255, 255, 255);

	double xx = (double)w / 4.0;
	double yy = (double)h / 4.0;
	int x;

	int colors[256];
	init_color_map(colors, im);

	cmpl c(c1, c2);

	unsigned char * picture = new unsigned char[s * s];
	memset(picture, white, s * s);

#pragma omp parallel for
	for (int x = 0; x < s; ++x) {
		double x1 = x / xx + a;
		for (int y = 0; y < s; ++y) {
			int iter = 0;
			double x2 = y / yy + b;

			cmpl z(x1, x2);

			//z^2 + c
			for (iter = 0; iter < 256; ++iter) {
				z = z * z + c;
				if (abs(z) > 2) break;
			}
			
			if (abs(z) < 2) {
				picture[x * s + y] = black;
			} else {
				picture[x * s + y] = colors[iter % 256];
			}
		}
	}

	for (int x = 0; x < s; ++x) {
		for (int y = 0; y < s; ++y) {
			gdImageSetPixel(im, x, (h - y), (int)picture[x * s + y]);
		}
	}

	delete [] picture;

	FILE * f = fopen("julia_filled.png", "wb");
	gdImagePng(im, f);
	fclose(f);
	gdImageDestroy(im);
}

static vector < pair < int, int > > circle(int xc, int yc, int r)
{
	vector < pair < int, int > > ret;
    int x, y;
    int d;

    x = 0;
    y = r;
    d = 3 - 2 * r;

	ret.push_back(make_pair( x,  y));
    while (y >= x) {
		ret.push_back(make_pair( x + xc,  y + yc));
		ret.push_back(make_pair( x + xc, -y + yc));
		ret.push_back(make_pair(-x + xc,  y + yc));
		ret.push_back(make_pair(-x + xc, -y + yc));
		ret.push_back(make_pair( y + xc,  x + yc));
		ret.push_back(make_pair( y + xc, -x + yc));
		ret.push_back(make_pair(-y + xc,  x + yc));
		ret.push_back(make_pair(-y + xc, -x + yc));
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y = y - 1;
        }
        x = x + 1;
    }
	return ret;
}

void julia_contour(int s, double c1, double c2)
{
	int colors[256];
	cmpl c(c1, c2);
	double a = - std::max(2.0, 2 * sqrt(std::abs(c)));
	double b = a;
	double l = -2.0 * a;

	int w = s;
	int h = s;
	int level = 20; //max iterations

	gdImagePtr im = gdImageCreate(w, h);
	int black = gdImageColorAllocate(im, 0, 0, 0);
	int white = gdImageColorAllocate(im, 255, 255, 255);
	gdImageFill(im, w - 1, h - 1, white);
	init_color_map(colors, im);
	double xx = (double)w / l;
	double yy = (double)h / l;

	//надо создать окружность с центром в s/2, s/2
	int radius = (int)((2.0 - a) * xx) / 2;
	std::cerr << s/2 << " " << radius << "\n";
	vector < pair < int, int > > r = circle(s/2, s/2, radius);
	vector < cmpl > z(r.size());

	for (uint i = 0; i < r.size(); ++i) {
		double x = (double)r[i].first  / xx + a;
		double y = (double)r[i].second / yy + b;
		z[i] = cmpl(x, y);
	}

	CmplVec Z (z);

	map < pair < int, int > , cmpl, LexicalCmp > new_points;
	typedef map < pair < int, int > , cmpl, LexicalCmp > ::iterator iter;

	for (uint k = 1; k < 20; ++k) {
		int col = colors[k];
		new_points.clear();
		for (uint i = 0; i < Z.size(); ++i) {
			int x = (int)((Z[i].real() - a) * xx);
			int y = (int)((Z[i].imag() - b) * yy);
			new_points.insert(make_pair(make_pair(x, y), Z[i]));
			gdImageSetPixel(im, x, (h - y), col);
		}

		Z.clear();
		for (iter it = new_points.begin(); it != new_points.end(); ++it)
		{
			Z.push_back(it->second);
		}

		Z = sqrt(Z - c);
		Z = CmplVec(Z, -Z);
//		gdImageFill(im, s/2, s/2, col);
	}

	FILE * f = fopen("julia_contour.png", "wb");
	gdImagePng(im, f);
	fclose(f);
	gdImageDestroy(im);	
}

void usage(const char * n)
{
	printf("usage:\n");
	printf("%s [-t type] [-s size] [-p pixels] [-c \"c1 + c2 i\"]\n", n);
	printf("-t -- draw type: \n");
	printf("\t\t filled  -- filled julia\n");
	printf("\t\t contour -- contours\n");
	printf("\t\t default -- txt (for vizualizer)\n");
	printf("-s -- size (size x size pixels) \n");
	printf("-c -- z ^ 2 + c, where c = c1 + c2 i \n");
	exit(0);
}

int main(int argc, char * argv[])
{
	int size = 512;
	int type = 0;
	//0 - default
	//1 - filled
	//2 - colored
	double c1 = -1.0;
	double c2 = 0.0;

	for (uint i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-s") && i < argc - 1) {
			size = atoi(argv[i + 1]);
		} else if (!strcmp(argv[i], "-c") && i < argc - 1) {
			double a1, a2;
			if (sscanf(argv[i + 1], "%lf + %lf i", &a1, &a2) == 2) {
				c1 = a1;
				c2 = a2;
			} else if (sscanf(argv[i + 1], "%lf i", &a2) == 1) {
				c1 = 0;
				c2 = a2;
			} else if (sscanf(argv[i + 1], "%lf", &a1) == 1) {
				c1 = a1;
				c2 = 0;
			}
		} else if (!strcmp(argv[i], "-t") && i < argc - 1) {
			if (!strcmp(argv[i + 1], "filled")) {
				type = 1;
			} else if (!strcmp(argv[i + 1], "contour")) {
				type = 2;
			}
		} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			usage(argv[0]);
		}
	}

	printf("c1 = %lf, c2 = %lf\n", c1, c2);

	switch (type) {
	case 1:
		julia(size, c1, c2);
		break;
	case 2:
		julia_contour(size, c1, c2);
		break;
	default:
		julia2(size, c1, c2);
		break;
	}

	return 0;
}
