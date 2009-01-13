/*$Id: viz_sphere.cpp 1867 2008-05-23 21:11:01Z manwe $*/

/* Copyright (c) 2005, 2008 Alexey Ozeritsky (Алексей Озерицкий)
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
#ifdef _WIN32
#define GLUT_BUILDING_LIB
#endif
#include <GL/glut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "viz_sphere.h"
#include "asp_excs.h"
#include "asp_interpolate.h"

#define _COLOR_ZONES 100

using namespace std;

Viz_Sphere::Viz_Sphere(const char * ctrl_points_file, int sType)
:Viz_Surface(ctrl_points_file, sPlain, dLines, 1)
{
	type = sType;
	//printf("type=%d\n", type);
	//full = 0;
	init();
}

Viz_Sphere::~Viz_Sphere()
{
	for (map < int, GLfloat * >::iterator it = m.begin(); it != m.end(); ++it)
	{
		delete [] it->second;
	}
}

void Viz_Sphere::init()
{
	n_phi = rows;
	n_la  = cols;
	zones = _COLOR_ZONES;

	if (type == sFull)
		d_phi = M_PI / (double) n_phi;
	else
		d_phi = M_PI / (double)(2 * (n_phi - 1) + 1);

	d_la    = 2. * M_PI / ((double) n_la);

	//FILE * f = fopen("test.out", "w");
	//for (int i = 0; i < n_phi; i++) {
	//	for (int j = 0; j < n_la; j++) {
	//		double x = data[i * n_la + j];
	//		if (x > 0)
	//			fprintf(f, " %.3e ", x);
	//		else
	//			fprintf(f, "%.3e ", x);
	//	}
	//	fprintf(f, "\n");
	//}
	//fclose(f);

	min =  1e20;
	max = -1e20;

	int n_phi_max = 512;
	int n_la_max  = 512;
	if (n_phi < n_phi_max || n_la < n_la_max) {
		double * ndata = new double[n_phi_max * n_la_max];

		bicubic_resample(n_la, n_phi, n_la_max, n_phi_max,
			data, ndata);
		delete [] data;
		data = ndata;
		n_phi = n_phi_max;
		n_la  = n_la_max;
		if (type == sFull)
			d_phi = M_PI / (double) n_phi;
		else
			d_phi = M_PI / (double)(2 * (n_phi - 1) + 1);

		d_la    = 2. * M_PI / ((double) n_la);
	}

	for (int i = 0; i < n_phi; i++) {
		for (int j = 0; j < n_la; j++) {
			float x = data[i * n_la + j];
			if (x < min) min = x;
			if (x > max) max = x;
		}
	}

	int b = zones / 3;
	int g = b + zones / 3;
	int r = zones;

	GLfloat * c;
	for (int i = 0; i < b; i ++) {
		c = new GLfloat[3];
		c[0] = 0.0; c[1] = 0.0; c[2] = (float) i / (float)b;
		m[i] = c;
	}

	for (int i = b; i < g; i ++) {
		c = new GLfloat[3];
		c[0] = 0.0; c[1] = (float) (i - b + 1) / (float)(g - b + 1); c[2] = 0.0;
		m[i] = c;
	}

	for (int i = g; i < r; i ++) {
		c = new GLfloat[3];
		c[0] = (float) (i - g + 1)/ (float)(r - g + 1); c[1] = 0.0; c[2] = 0.0;
		m[i] = c;
	}

	init_sphere();
}

int Viz_Sphere::color(float f) {
	for (int i = 0; i < zones; i++) {
		float b = min + (float) i * (max - min) / (float) zones;
		if (f < b)
			return i;
	}
	return zones - 1;
}

void Viz_Sphere::init_sphere()
{
	GLdouble *f = data;

	sphere = glGenLists(1);
	glNewList(sphere, GL_COMPILE);

	glBegin(GL_TRIANGLES);

	double x1, x2, x3;
	double y1, y2, y3;
	double z1, z2, z3;

	double p1, p2, p3;
	double l1, l2, l3;

	double f1, f2, f3;
	double s = 0.0;

	if (type == sFull) {
		s = 0.5 * (d_phi - M_PI);
	}

	for (int i = 0; i < n_phi - 1; i++) {
		for (int j = 0; j < n_la; j++) {
			if (type == sFlat) {
				p1 = p2 = (n_phi - i) * 1.0 / n_phi;
				p3 = (n_phi - (i + 1)) * 1.0 / n_phi;
			} else {
				p1 = p2 = s + i * d_phi;
				p3 = s + (i + 1) * d_phi;
			}

			l1 = l3 = j * d_la;
			l2 = (j + 1) % n_la * d_la;

			f1 = f[i * n_la + j];
			f2 = f[i * n_la + (j + 1) % n_la];
			f3 = f[(i + 1) * n_la + j];

			if (type == sFlat) {
				z1 = z2 = z3 = 0;

				x1 = cos(l1) * p1; y1 = sin(l1) * p1;
				x2 = cos(l2) * p2; y2 = sin(l2) * p2;
				x3 = cos(l3) * p3; y3 = sin(l3) * p3;
			} else {
				z1 = sin(p1); z2 = sin(p2); z3 = sin(p3);
				x1 = cos(l1) * cos(p1); y1 = sin(l1) * cos(p1);
				x2 = cos(l2) * cos(p2); y2 = sin(l2) * cos(p2);
				x3 = cos(l3) * cos(p3); y3 = sin(l3) * cos(p3);
			}

			glColor3fv(m[color(f1)]);
			glVertex3f(x1, y1, z1);

			glColor3fv(m[color(f2)]);
			glVertex3f(x2, y2, z2);

			glColor3fv(m[color(f3)]);
			glVertex3f(x3, y3, z3);

			p1 = p3; l1 = l3; f1 = f3; z1 = z3;
			x1 = x3; y1 = y3;
			l3 = (j + 1) % n_la * d_la;
			
			f3 = f[(i + 1) * n_la + (j + 1) % n_la];
			if (type == sFlat) {
				z3 = 0;
				x3 = cos(l3) * p3; y3 = sin(l3) * p3;
			} else {
				z3 = sin(p3);
				x3 = cos(l3) * cos(p3); y3 = sin(l3) * cos(p3);
			}

			glColor3fv(m[color(f1)]);
			glVertex3f(x1, y1, z1);

			glColor3fv(m[color(f2)]);
			glVertex3f(x2, y2, z2);

			glColor3fv(m[color(f3)]);
			glVertex3f(x3, y3, z3);
		}
	}
	glEnd();
	glEndList();
}

void Viz_Sphere::draw()
{
	glPushMatrix();
	glCallList(sphere);
	glPopMatrix();
	glFlush();
}
