#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "c2_fbm_hcalc.h"

using namespace std;

static double
STD(const vector < double > & X)
{
	double M  = 0;
	double M2 = 0;
	double d;

	for (size_t i = 0; i < X.size(); ++i) {
		M  = (i * M + X[i]) / (double)(i + 1);
		M2 = (i * M2 + X[i] * X[i]) / (double)(i + 1);
	}

	d = M2 - M * M;
	return sqrt(d);
}

static double
sum(const vector < double > & a)
{
	double s = 0.0;
	for (size_t i = 0; i < a.size(); ++i) {
		s += a[i];
	}
	return s;
}

static double
sum2(const vector < double > & a)
{
	double s = 0.0;
	for (size_t i = 0; i < a.size(); ++i) {
		s += a[i] * a[i];
	}
	return s;
}

static double
scalar(const vector < double > & a, const vector < double > & b)
{
	double s = 0.0;
	for (size_t i = 0; i < a.size(); ++i) {
		s += a[i] * b[i];
	}
	return s;
}

static double
mnk_angle(const vector < double > & ksi, const vector < double > & nu)
{
	double A11 = ksi.size();
	double A12 = sum(ksi);
	double A21 = A12;
	double A22 = sum2(ksi);

	double d1  = sum(nu);
	double d2  = scalar(ksi, nu);

	double D  = A11 * A22 - A21 * A12;
	double D1 = d1 * A22 - d2 * A12;
	double D2 = A11  * d2 - A21 * d1;

	return D2 / D;
}

double hcalc(vector < double > & X, int pmax)
{
	size_t L = X.size();
	vector < double > dX(L - pmax);
	vector < double > ksi(pmax);
	vector < double > nu(pmax);
	double s_p;

	assert(((L - pmax) > 1));

	for (size_t p = 0; p < pmax; ++p) {
		for (size_t i = 0; i < L - pmax; ++i) {
			dX[i] = X[i + (p + 1)] - X[i];
		}
		s_p    = STD(dX);
		ksi[p] = log10((double)(p + 1));
		nu[p]  = log10((double)s_p);
	}
#if 0
	for (size_t p = 0; p < pmax; ++p) {
		printf("{%lf %lf} ", ksi[p], nu[p]);
	}
	printf("\n");
#endif
	return mnk_angle(ksi, nu);
}

#ifdef min
#undef min
#endif

static void
usage(const char * name)
{
	fprintf(stderr, "usage: %s [-f file]\n", name);
	exit(1);
}

int main(int argc, char * argv[])
{
	FILE * f = stdin;
	double d;
	double m;
	int i;

	for (i = 0; i < argc; ++i) {
		if (!strcmp(argv[i], "-h")
				|| !strcmp(argv[i], "--help"))
		{
			usage(argv[0]);
		} else if (!strcmp(argv[i], "-f")) {
			if (i < argc - 1) {
				f = fopen(argv[i + 1], "rb");
				if (!f) {
					fprintf(stderr, "%s not found\n", 
							argv[i + 1]);
					usage(argv[0]);
				}
			} else {
				usage(argv[0]);
			}
		}
	}

	vector < double > X;
	while(fscanf(f, "%lf", &d) == 1) {
		X.push_back(d);
	}
	printf("read %lu values\n", X.size());
#if 1
	m = sum(X) / X.size();
	printf("m = %lf\n", m);
	for (size_t i = 0; i < X.size(); ++i) {
		X[i] -= m;
	}
#endif
	printf("%.16lf\n", hcalc(X, std::min(300, (int)X.size() / 10)));
}

