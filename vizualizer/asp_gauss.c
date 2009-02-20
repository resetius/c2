/*  $Id$  */

/* Copyright (c) 2000, 2001, 2002, 2003, 2004 Alexey Ozeritsky
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

/**
 *  Алгоритмы линейной алгербры.
 *  Решение линейных систем и обращения матриц
 */

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "asp_gauss.h"

static void make_identity_matrix(double *X, int n) {
	int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			if (i == j) X[i * n + j] = 1.0;
			else X[i * n + j] = 0.0;
		}
	}
}

static void load_identity_matrix(double **X, int n) {
	*X = (double *) malloc(n * n * sizeof(double));
	make_identity_matrix(*X, n);
}


/**
 * Обращение матрицы.
 * @param n размерность
 * @param A исходная
 * @param X обратная
 * @return
 */
void inverse_general_matrix_my(double *Dest, double * Source, int n) {
    int i, j, k;
    int r;
	double * A = (double*) malloc(n * n * sizeof(double)); 
	double * X;
	int need_to_free_X = 0;

	memcpy(A, Source, n * n * sizeof(double));

	
	if (Dest == Source) {
		load_identity_matrix(&X, n);
		need_to_free_X = 1;
	} else {
		X = Dest;
		make_identity_matrix(X, n);
	}

    for (j = 0; j < n; j++) {
    	double a;
     /*поиск главного значения*/
        findmain(&r,j,n,A);
        swop(r,j,n,A);
        swop2(r,j,n,X);

        a = A[j * n + j];
        for (i = j + 1; i < n; i++) {
            double ba = A[i * n + j] / a;

            for (k = j; k < n; k++)
                A[i * n + k] -= A[j * n + k] * ba;

            for (k = 0; k < n; k++)
                X[i * n + k] -= X[j * n + k] * ba;
        }
    }

    for (j = n - 1; j >= 0; j--) {
        double a = A[j * n + j];
        for (i = j - 1; i >= 0; i--) {
            double ba = A[i * n + j] / a;
            for (k = n - 1; k >= j; k--)
                A[i * n + k] -= A[j * n + k] * ba;

            for (k = n - 1; k >= 0; k--)
                X[i * n + k] -= X[j * n + k] * ba;
        }
    }

    for(i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            X[i * n + j] *= 1.0 / A[i * n + i];
        }
    }

	if (need_to_free_X) {
		memcpy(Dest, X, n * n * sizeof(double));
		free(X);
	}
	free(A);
}

void  solve_tdiag_linear_my(double * B, double*A1, double *A2, double *A3, int n) {
	int i, j = 0;
	double a;

	for (j = 0; j < n - 1; j++) {	
		a = A1[j] / A2[j];
		A1[j] = 0;
		A2[j + 1] -= A3[j] * a;
		B[j + 1] -= B[j] * a;
	}

	for (j = n - 1; j > 0; j--) {
//		if (fabs(A2[j]) > 1e-16) {
			a = A3[j - 1] / A2[j];
			B[j - 1] -= B[j] * a;
//		}
	}

	for (i = 0; i < n; i++) {
//		if (fabs(A2[i]) > 1e-16)
			B[i] *= 1 / (A2[i]);
	}
}

void inverse_tdiag_matrix_my(double *Dest, double * A1,double *A2,double *A3, int n) {
	int i, j = 0, k;
	double a;
	double *X = Dest;

	make_identity_matrix(X, n);

	for (j = 0; j < n - 1; j++) {	
		a = A1[j] / A2[j];
		A1[j] = 0;
		A2[j + 1] -= A3[j] * a;
		//for (i = j + 1; i < n; i++) {
		//	for (k = 0; k < n; k++)
		//		X[i * n + k] -= X[j * n + k] * a;
		//}
		for (k = 0; k < n; k++)
			X[(j + 1) * n + k] -= X[j * n + k] * a;
		//B[j + 1] -= B[j] * a;
	}
    
	for (j = n - 1; j > 0; j--) {	
		a = A3[j - 1] / A2[j];
		//for (i = j - 1; i >= 0; i--) {
		//	for (k = n - 1; k >= 0; k--)
		//		X[i * n + k] -= X[j * n + k] * a;
		//}
		for (k = n - 1; k >= 0; k--)
			X[(j - 1) * n + k] -= X[j * n + k] * a;
		
		//B[j - 1] -= B[j] * a;
	}

	for(i = 0; i < n; i++) {
		for(j = 0; j < n; j++) {
			X[i * n + j] *= 1.0 / A2[i];
		}
	}    
	//for (i = 0; i < n; i++)
	//	B[i] *= 1 / (A2[i]);
}

/**
 * поиск главного значения по столбцу
 */
double findmain(int*r, int k, int n, double*A) {
	int j;
	double max = A[k * n + k];
	double c;
	(*r)=k;
	for (j = k; j < n; j++) {
		c = fabs(A[j*n+k]);
		if (max < c) {
			(*r) = j;
			max  = c;
		}
	}
	return max;
}

/**
 * перестановка k'й и r'й строки
 * начиная с индекса k
 */
void  swop(int r,int k,int n,double*A) {
	int i;
	double temp;
	for(i = k; i < n; i++) {
		temp = A[k*n+i]; A[k*n+i] = A[r*n+i]; A[r*n+i] = temp;
	}
}

/**
 * перестановка k'й и r'й строки
 * начиная с индекса 0
 */
void  swop2(int r,int k,int n,double*A) {
	int i;
	double temp;
	if(r!=k)
		for(i=0;i<n;i++) {
			temp=A[k*n+i];A[k*n+i]=A[r*n+i];A[r*n+i]=temp;
		}
}

double nev(double*A,double*B,int n) {
    int i,j,k;
    //double*C;
    double max=0,temp,cij;
    for(i=0;i<n;i++) {
        temp=0;
        for(j=0;j<n;j++) {
            cij=0;

            for(k=0;k<n;k++) cij+=A[i*n+k]*B[k*n+j];
            if(i==j) cij-=1;
            temp+=fabs(cij);
        }
        if(max<temp) max=temp;
    }
    return max;
}
