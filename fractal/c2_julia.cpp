#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gd.h>

#include <complex>
#include <iostream>
#include <vector>

using namespace std;
typedef complex < double > cmpl;

class CmplVec {
	vector < cmpl > d;
	friend CmplVec sqrt(const CmplVec & v);

public:
	CmplVec() {}

	CmplVec(const cmpl & v): d(1) {
		d[0] = v;
	}

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
void julia2(int s, int p, double c1, double c2)
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
void julia(int s, int p, double c1, double c2)
{
	// [-2, 2]
	// [-2, 2]
	// 4x4

	double a = -2.0;
	double b = -2.0;
	double step = (double)4.0 / (double)p;
	int w = s;
	int h = s;
	gdImagePtr im = gdImageCreate(w, h);
	int black = gdImageColorAllocate(im, 0, 0, 0);
	int white = gdImageColorAllocate(im, 255, 255, 255);
	gdImageFill(im, w - 1, h - 1, white);

	double xx = (double)w / 4.0;
	double yy = (double)h / 4.0;

	cmpl c(c1, c2);

#pragma omp parallel for
	for (int m = 0; m < p; ++m) {
		double x1 = a + (double)m * step;
		for (int n = 0; n < p; ++n) {
			double x2 = (double)b + (double)(n) * step;

			cmpl z(x1, x2);

//			std::cerr << "(c1, c2)\t" << c1 << ", " << c2 << "\n";

			//z^2 + c
			for (int iter = 0; iter < 100; ++iter) {
				z = z * z + c;
				if (abs(z) > 4) break;
			}

			if (abs(z) < 4) {
//				cerr << "put pixel " << (int)c1 << " " << (h - (int)c2) << "\n";

				int x = (int)((x1 - a) * xx);
				int y = (int)((x2 - b) * yy);
				gdImageSetPixel(im, x, (h - y), black);
			}
		}
	}

	FILE * f = fopen("julia.png", "wb");
	gdImagePng(im, f);
	fclose(f);
	gdImageDestroy(im);
}

void usage(const char * n)
{
	printf("usage:\n");
	printf("%s [-t type] [-s size] [-p pixels] [-c \"c1 + c2 i\"]\n", n);
//	printf("-t -- points save type: \n");
//	printf("\t\t png -- PNG\n");
//	printf("\t\t default -- txt (for vizualizer)\n");
	printf("-s -- size (size x size pixels) \n");
	printf("-c -- z ^ 2 + c, where c = c1 + c2 i \n");
	printf("-p -- pixels \n");
	printf("-f -- filled set\n");
	exit(0);
}

int main(int argc, char * argv[])
{
	int size = 512;
	int p    = 512;
	int type = 0; //txt
	bool filled = false;
	double c1 = -1.0;
	double c2 = 0.0;

	for (uint i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-s") && i < argc - 1) {
			size = atoi(argv[i + 1]);
		} else if (!strcmp(argv[i], "-p") && i < argc - 1) {
			p = atoi(argv[i + 1]);
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
			if (!strcmp(argv[i + 1], "png")) {
				type = 2;
			}
		} else if (!strcmp(argv[i], "-f")) {
			filled = true;
		} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			usage(argv[0]);
		}
	}

	printf("c1 = %lf, c2 = %lf\n", c1, c2);
	(filled) ?  julia(size, p, c1, c2) : julia2(size, p, c1, c2);

	return 0;
}
