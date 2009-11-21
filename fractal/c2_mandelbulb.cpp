#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <bitset>
#include <vector>
#include <complex>

#include <gd.h>

using namespace std;

typedef complex < float > cmpl;

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

int do_all(int l, int w, int h, int order)
{
	vector < bool > mask((long)l * (long)w * (long)h);
	float s1 = -2;
	float s2 =  2;

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
//					printf("%d ", k);
					mask[offset] = true;
				} 
			}
//			printf("\n");
		}
	}

	gdImagePtr im = gdImageCreateTrueColor(w, h);
	for (int i = 0; i < l; ++i) {
		for (int j = 0; j < w; ++j) {
			for (int k = 0; k < h; ++k) {
				int offset = i * w * h + j * h + k;
				gdImageSetPixel(im, i, j, gdImageColorExact(im, 255, 255, 255));
				if (mask[offset]) {
					int color = ((double)k / h) * 256.;
	//				printf("%d ", k);
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

int main(int argc, char ** argv)
{
	int l, w, h, order;
	l = w = h = atoi(argv[1]);
	order = atoi(argv[2]);
	do_all(l, w, h, order);
}

