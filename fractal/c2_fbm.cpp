#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <gd.h>

#include <vector>
#include <complex>
#include <iostream>

using namespace std;
typedef complex < double > cmpl;

//fractal Brownian motion

void init()
{
	srand(time(0));
}

double uniform()
{
	return (double)rand() / (double)RAND_MAX;
}

double normal()
{
	double r   = uniform();
	double phi = uniform();
	double z0  = cos(2 * M_PI * phi) * sqrt(-2.0 * log(r));

	return r;
}

inline unsigned rev(unsigned x)
{
	x = (x & 0x55555555) << 1 | (x >> 1) & 0x55555555;
	x = (x & 0x33333333) << 2 | (x >> 2) & 0x33333333;
	x = (x & 0x0f0f0f0f) << 4 | (x >> 4) & 0x0f0f0f0f;
	x = (x << 24) | ((x & 0xff00) << 8) |
		((x >> 8) & 0xff00) | (x >> 24);
	return x;
}

void bit_reverse_copy(const vector < cmpl > & a, vector < cmpl > & A, uint n1)
{
	uint sh = 32 - n1;
	uint n  = a.size();
//	cerr << "N=" << n  << "\n";

//	cerr << n1 << "\n";
	for (uint k = 0; k < n; ++k) {
//		cerr << "k = " << k << " " << (rev(k) >> sh) << "\n";
		A[(rev(k) >> sh)] = a[k];
	}
}

vector < cmpl > fft(vector < cmpl> & a)
{
	vector < cmpl > A(a.size());

	uint N = a.size();

//	cerr << "N=" << N  << "\n";
	
	uint n = 0;
	while ((N >>= 1)) {
		++n;
	}
	N = a.size();

	bit_reverse_copy(a, A, n);

//	cerr << N << " " << n << "\n";
	cmpl i(0.0, 1.0);
	for (uint s = 1; s <= n; ++s) {
		uint m  = 1 << s; //2 ^ s;
		cmpl wm = exp(2.0 * M_PI * i / (double)m);

		for (uint k = 0; k <= N - 1; k += m) {
			cmpl w = 1;
			for (uint j = 0; j <= m / 2 - 1; ++j) {
				cmpl t = w * A[k + j + m / 2];
				cmpl u = A[k + j];
				A[k + j] = u + t;
				A[k + j + m / 2] = u - t;
				w = w * wm;
			}
		}
	}

	return A;
}

void fbd(int N, double H)
{
	vector < cmpl > X1(N);
	vector < cmpl > X(N);
	cmpl i (0.0, 1.0);

	X1[0] = normal();
	for (uint j = 1; j <= N / 2 - 1; ++j) {
		X1[j] = normal() * exp(2.0 * M_PI * i * uniform()) / pow((double)j, H + 0.5);
	}
	X1[N / 2] = normal() * exp(2.0 * M_PI * i * uniform()) / pow((double)(N / 2), H + 0.5);
	for (uint j = N / 2 + 1; j <= N - 1; ++j) {
		X1[j] = conj(X[N - j]);
	}

	X = fft(X1);

	for (uint j = 0; j < N; ++j) {
		cerr << X[j] << " ";
	}
	cerr << "\n";
}

int main (int argc, char * argv[])
{
	int N = 512;
	double H = 0.5;

	init();
	for (uint i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-N") && i < argc - 1) {
			N = atoi(argv[i + 1]);
		} else if (!strcmp(argv[i], "-H") && i < argc - 1) {
			double a;
			if (sscanf(argv[i + 1], "%lf", &a) == 1) {
				H = a;
			}
		}
	}

	printf("N = %d, H = %lf\n", N, H);
	fbd(N, H);
	
	return 0;
}
