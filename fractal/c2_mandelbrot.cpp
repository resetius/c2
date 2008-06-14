#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gd.h>

#include <complex>
#include <iostream>

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

void mandelbrot(int s)
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

	int colors[256];
	init_color_map(colors, im);

	unsigned char * picture = new unsigned char[s * s];
	memset(picture, white, s * s);

#pragma omp parallel for
	for (int x = 0; x < s; ++x) {
		double c1 = x / xx + a;
		for (int y = 0; y < s; ++y) {
			int iter = 0;
			double c2 = y / yy + b;

			cmpl z(0, 0);

			//z^2 + c
			for (iter = 0; iter < 256; ++iter) {
				z = z * z + cmpl(c1, c2);
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

	FILE * f = fopen("mandelbrot.png", "wb");
	gdImagePng(im, f);
	fclose(f);
	gdImageDestroy(im);
}

void usage(const char * n)
{
	printf("usage:\n");
	printf("%s [-t type] [-s size]\n", n);
	printf("-t -- points save type: \n");
	printf("\t\t png -- PNG\n");
	printf("\t\t default -- txt (for vizualizer)\n");
	printf("-s -- size (size x size pixels) \n");
	exit(0);
}

int main(int argc, char * argv[])
{
	int size = 512;
	int type = 0; //txt

	for (uint i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-s") && i < argc - 1) {
			size = atoi(argv[i + 1]);
		} else if (!strcmp(argv[i], "-t") && i < argc - 1) {
			if (!strcmp(argv[i + 1], "png")) {
				type = 2;
			}
		} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			usage(argv[0]);
		}
	}

	mandelbrot(size);

	return 0;
}
