#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <bitset>
#include <vector>
#include <complex>

using namespace std;

typedef complex < float >   cmpl;
typedef vector < char > volume_t;

struct Point
{
	short x;
	short y;
	short z;

	Point(short i, short j, short k): x(i), y(j), z(k) {}
	Point(): x(0), y(0), z(0) {}
};

typedef vector < Point > boundary_t;

#ifndef WIN32
#include <gd.h>
void draw(volume_t & mask) {
	gdImagePtr im = gdImageCreateTrueColor(w, h);
	for (int i = 0; i < l; ++i) {
		for (int j = 0; j < w; ++j) {
			for (int k = 0; k < h; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				gdImageSetPixel(im, i, j, gdImageColorExact(im, 255, 255, 255));
				if (mask[offset]) {
					int color = ((double)k / h) * 256.;
					gdImageSetPixel(im, i, j, gdImageColorExact(im, 0, 0, color));
					break;
				}
			}
		}
	}
	FILE * f = fopen("mandelbulb.png", "wb");
	gdImagePng(im, f);
	fclose(f);
	gdImageDestroy(im);
}

/*
void draw_bnd(boundary_t & bnd) {
	gdImagePtr im = gdImageCreateTrueColor(w, h);
	for (boundary_t::iterator it = bnd.begin(); it < bnd.end(); ++it)
		long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
		gdImageSetPixel(im, i, j, gdImageColorExact(im, 255, 255, 255));
		int color = ((double)k / h) * 256.;
		gdImageSetPixel(im, i, j, gdImageColorExact(im, 0, 0, color));
	}

	FILE * f = fopen("mandelbulb.png", "wb");
	gdImagePng(im, f);
	fclose(f);
	gdImageDestroy(im);
}
*/

#endif

float ipow(float a, int p)
{
	float r = 1.0;
	int i;
	for (i = 0; i < p; ++i)
	{
		r *= a;
	}
	return r;
}

void init_boundary(boundary_t & bnd, volume_t & vol, int l, int w, int h)
{
	bnd.reserve(6 * w * h);
	back_insert_iterator < boundary_t > it = std::back_inserter(bnd);

	for (int i = 0; i < 1; ++i) {
		for (int j = 1; j < w - 1; ++j) {
			for (int k = 1; k < h - 1; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				vol[offset] = -1;
				*it++ = Point(i, j, k);
			}
		}
	}

	for (int i = l - 1; i < l; ++i) {
		for (int j = 1; j < w - 1; ++j) {
			for (int k = 1; k < h - 1; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				vol[offset] = -1;
				*it++ = Point(i, j, k);
			}
		}
	}

	for (int i = 1; i < l - 1; ++i) {
		for (int j = 0; j < 1; ++j) {
			for (int k = 1; k < h - 1; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				vol[offset] = -1;
				*it++ = Point(i, j, k);
			}
		}
	}

	for (int i = 1; i < l - 1; ++i) {
		for (int j = w - 1; j < w; ++j) {
			for (int k = 1; k < h - 1; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				vol[offset] = -1;
				*it++ = Point(i, j, k);
			}
		}
	}

	for (int i = 1; i < l - 1; ++i) {
		for (int j = 1; j < w - 1; ++j) {
			for (int k = 0; k < 1; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				vol[offset] = -1;
				*it++ = Point(i, j, k);
			}
		}
	}

	for (int i = 1; i < l - 1; ++i) {
		for (int j = 1; j < w - 1; ++j) {
			for (int k = h - 1; k < h; ++k) {
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;
				vol[offset] = -1;
				*it++ = Point(i, j, k);
			}
		}
	}
}

bool build_boundary_(boundary_t & answer, boundary_t & bnd, volume_t & vol, int l, int w, int h)
{
	boundary_t new_bnd;
	new_bnd.reserve(bnd.size());
	back_insert_iterator < boundary_t > insert = std::back_inserter(new_bnd);
	back_insert_iterator < boundary_t > answ   = std::back_inserter(answer);
	bool updated = false;

	for (boundary_t::iterator it = bnd.begin(); it != bnd.end(); ++it)
	{
		int x = it->x;
		int y = it->y;
		int z = it->z;
		long offset = (long)x * (long)w * (long)h + (long)y * (long)h + (long)z;
		if (vol[offset] == 2) {
			// already on the surface set
			continue;
		}

		for (int i = -1; i <= 1; ++i) {
			for (int j = -1; j <= 1; ++j) {
				for (int k = -1; k <= 1; ++k) {
					int x = i + it->x;
					int y = j + it->y;
					int z = k + it->z;
					if (!(0 <= x && x < l)) {
						continue;
					}
					if (!(0 <= y && y < w)) {
						continue;
					}
					if (!(0 <= z && z < h)) {
						continue;
					}
					long offset = (long)x * (long)w * (long)h + (long)y * (long)h + (long)z;

					if (vol[offset] < 0) {
						continue;
					}

					if (vol[offset] == 1) {
						vol[offset] = 2;
						*answ++     = Point(x, y, z);
					} else {
						vol[offset] = -1;
						*insert++   = Point(x, y, z);
						updated     = true;
					}
				}
			}
		}
	}
	new_bnd.swap(bnd);
	return updated;
}

void build_boundary(boundary_t & answer, boundary_t & bnd, volume_t & vol, int l, int w, int h)
{
	while (build_boundary_(answer, bnd, vol, l, w, h))
	{
	}
}

void do_all(int l, int w, int h, int order)
{
	volume_t mask(l * w * h);
	boundary_t bnd;
	boundary_t answer;

	float s1 = -2.0f;
	float s2 =  2.0f;

	float xx = (float)l / (s2 - s1);
	float yy = (float)w / (s2 - s1);
	float zz = (float)h / (s2 - s1);

#pragma omp parallel for
	for (int i = 0; i < l; ++i) {
		float c1 = i / xx + s1;
		for (int j = 0; j < w; ++j) {
			float c2 = j / yy + s1;
			for (int k = 0; k < h; ++k) {
				float c3 = k / zz + s1; 
				long offset = (long)i * (long)w * (long)h + (long)j * (long)h + (long)k;

				float z1 = 0, z2 = 0, z3 = 0;
				for (int iter = 0; iter < 256; ++iter)
				{
					float r     = sqrtf(z1*z1+z2*z2+z3*z3);
					float theta = atan2f(sqrtf(z1*z1+z2*z2), z3);
					float phi   = atan2f(z2, z1);
					float ro    = ipow(r, order);

					z1 = ro * sinf(theta*order) * cosf(phi*order) + c1;
					z2 = ro * sinf(theta*order) * sinf(phi*order) + c2;
					z3 = ro * cosf(theta*order) + c3;

					if (sqrtf(z1*z1+z2*z2+z3*z3) > 2) {
						break;
					}
				}

				if (sqrtf(z1*z1+z2*z2+z3*z3) < 2) {
					mask[offset] = 1;
				}
			}
		}
	}

	fprintf(stderr, "init bnd\n");
	init_boundary(bnd, mask, l, w, h);
	fprintf(stderr, "build bnd\n");
	build_boundary(answer, bnd, mask, l, w, h);

	fprintf(stderr, "draw\n");
#ifndef WIN32
	draw(mask);
#endif

	fprintf(stderr, "ok\n");
}

int main(int argc, char ** argv)
{
	int l, w, h, order;
	if (argc > 1) {
		l = w = h = atoi(argv[1]);
	} else {
		l = w = h = 64;
	}

	if (argc > 2) {
		order = atoi(argv[2]);
	} else {
		order = 8;
	}
	do_all(l, w, h, order);
}

