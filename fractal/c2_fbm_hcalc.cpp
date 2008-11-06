#include <math.h>
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
	double s = 0;
	for (size_t i = 0; i < a.size(); ++i) {
		s += a[i];
	}
	return s;
}

static double
sum2(const vector < double > & a)
{
	double s;
	for (size_t i = 0; i < a.size(); ++i) {
		s += a[i] * a[i];
	}
	return s;
}

static double
scalar(const vector < double > & a, const vector < double > & b)
{
	double s;
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

	for (size_t p = 1; p < pmax; ++p) {
		for (size_t i = 0; i < L - pmax; ++i) {
			dX[i] = X[i + p] - X[i];
		}
		s_p    = STD(dX);
		ksi[p] = log10((double)p);
		nu[p]  = log10((double)s_p);
	}
/*
	for (size_t p = 0; p < pmax; ++p) {
		printf("{%lf %lf} ", ksi[p], nu[p]);
	}
	printf("\n");
*/
	return mnk_angle(ksi, nu);
}

int main(int agrc, char * argv[])
{
	const char * file = argv[1];
	FILE * f = fopen(file, "rb");
	double d;
	double m;
	vector < double > X;
	while(fscanf(f, "%lf", &d) == 1) {
		X.push_back(d);
	}
	printf("read %lu values\n", X.size());
	m = sum(X) / X.size();
	printf("m = %lf\n", m);
	for (size_t i = 0; i < X.size(); ++i) {
		X[i] -= m;
	}
	printf("%.16lf\n", hcalc(X, 30768));
}

