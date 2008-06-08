#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gd.h>

#include <complex>
#include <iostream>

using namespace std;
typedef complex < double > cmpl;

void mandelbrot(int s, int p)
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

#pragma omp parallel for
	for (int m = 0; m < p; ++m) {
		double c1 = a + (double)m * step;
		for (int n = 0; n < p; ++n) {
			double c2 = (double)b + (double)(n) * step;

			cmpl z(0, 0);

//			std::cerr << "(c1, c2)\t" << c1 << ", " << c2 << "\n";

			//z^2 + c
			for (int iter = 0; iter < 100; ++iter) {
				z = z * z + cmpl(c1, c2);
				if (abs(z) > 4) break;
			}

			if (abs(z) < 4) {
//				cerr << "put pixel " << (int)c1 << " " << (h - (int)c2) << "\n";

				int x = (int)((c1 - a) * xx);
				int y = (int)((c2 - b) * yy);
				gdImageSetPixel(im, x, (h - y), black);
			}
		}
	}

	FILE * f = fopen("mandelbrot.png", "wb");
	gdImagePng(im, f);
	fclose(f);
	gdImageDestroy(im);
}

void usage(const char * n)
{
	printf("usage:\n");
	printf("%s [-t type] [-s size] [-p pixels]\n", n);
	printf("-t -- points save type: \n");
	printf("\t\t png -- PNG\n");
	printf("\t\t default -- txt (for vizualizer)\n");
	printf("-s -- size (size x size pixels) \n");
	printf("-p -- pixels \n");
	exit(0);
}

int main(int argc, char * argv[])
{
	int size = 512;
	int p    = 512;
	int type = 0; //txt

	for (uint i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-s") && i < argc - 1) {
			size = atoi(argv[i + 1]);
		} else if (!strcmp(argv[i], "-p") && i < argc - 1) {
			p = atoi(argv[i + 1]);
		} else if (!strcmp(argv[i], "-t") && i < argc - 1) {
			if (!strcmp(argv[i + 1], "png")) {
				type = 2;
			}
		} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			usage(argv[0]);
		}
	}

	mandelbrot(size, p);

	return 0;
}
