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

#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

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

	for (uint k = 0; k < n; ++k) {
		A[(rev(k) >> sh)] = a[k];
	}
}

vector < cmpl > fft(vector < cmpl> & a, int sgn)
{
	vector < cmpl > A(a.size());

	uint N = a.size();

	uint n = 0;
	while ((N >>= 1)) {
		++n;
	}
	N = a.size();

	bit_reverse_copy(a, A, n);

	cmpl i(0.0, 1.0);
	for (uint s = 1; s <= n; ++s) {
		uint m  = 1 << s; //2 ^ s;
		cmpl wm = exp(sgn * 2.0 * M_PI * i / (double)m);

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

#ifdef HAVE_FFTW
#include <fftw3.h>

vector < cmpl > fft_fftw(vector < cmpl> & a, int sgn)
{
	fftw_complex * in  = new fftw_complex[a.size()];
	fftw_complex * out = new fftw_complex[a.size()];

	for (uint i = 0; i < a.size(); ++i) {
		in[i][0] = a[i].real();
		in[i][1] = a[i].imag();
	}

	fftw_plan plan = fftw_plan_dft_1d(a.size(),
									  &in[0], &out[0],
									  sgn, 0);

	fftw_execute(plan);

	vector < cmpl > A; A.reserve(a.size());
	for (uint i = 0; i < a.size(); ++i) {
		A.push_back(cmpl(out[i][0], out[i][1]));
	}
	
	fftw_destroy_plan(plan);
	delete [] in;
	delete [] out;

	return A;
}
#endif

vector < cmpl > fft_lob(vector < cmpl > & X1)
{
	vector < cmpl > X(X1.size());
	cmpl i (0.0, 1.0);
	int N = X1.size();

	for (int n = 0; n < N; ++n) {
		X[n] = cmpl(0, 0);

		for (int k = 0; k < N; ++k) {
			cmpl p1 = i;
			p1 *= 2.0 * M_PI * k * n / N;
			X[n] += X1[k] * std::exp(p1);
		}
	}

	return X;
}

void fbd(int N, double H)
{
	vector < cmpl > X1(N);
	vector < cmpl > X(N);
	cmpl i (0.0, 1.0);

	X1[0] = normal();
	for (uint j = 1; j <= N / 2 - 1; ++j) {
		X1[j] = normal() * exp(i * 2.0 * M_PI * uniform()) / pow((double)j, H + 0.5);
	}
	X1[N / 2] = normal() * exp(i * 2.0 * M_PI * uniform()) / pow((double)(N / 2), H + 0.5);
	X1[N / 2].imag() = 0;
	for (uint j = N / 2 + 1; j <= N - 1; ++j) {
		X1[j] = conj(X1[N - j]);
	}

#ifdef HAVE_FFTW
	X = fft_fftw(X1, -1);
#else
	X = fft(X1, -1);
#endif

#if 0
	for (uint j = 0; j < N; ++j) {
		cerr << X[j] << " ";
	}
	cerr << "\n";
#endif

	for (uint j = 0; j < N; ++j) {
		cout << X[j].real() << "\n";
	}

#if 0
	X = fft(X1, -1);

	for (uint j = 0; j < N; ++j) {
		cerr << X[j] << " ";
	}
	cerr << "\n";
#endif
}

static void
usage(const char * name)
{
	fprintf(stderr, "usage: %s [-H H] [-N N]\n", name);
	exit(1);
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
		} else if (!strcmp(argv[i], "-h")
				|| !strcmp(argv[i], "--help"))
		{
			usage(argv[0]);
		}
	}

	fprintf(stderr, "N = %d, H = %lf\n", N, H);
	fbd(N, H);
	
	return 0;
}

