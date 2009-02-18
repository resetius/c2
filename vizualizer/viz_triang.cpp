/*$Id$*/

/* Copyright (c) 2007 Alexey Ozeritsky (Алексей Озерицкий)
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
#include <math.h>

#include <cstdio>
#include <iostream>
#include <sstream>
#include <set>

#include "asp_excs.h"
#include "asp_interpolate.h"

#include "viz_triang.h"

using namespace std;

Viz_Triang::Viz_Triang (const char * file)
		: fname_ (file), mode_(dFill)
{
	load_file();
	normalize();
	build_isolines();
	gen_lists();

	hidden_ = false;
	oType_  = oGeneral;
	dMode_  = dFill;
}

Viz_Triang::~Viz_Triang()
{
}

void Viz_Triang::normalize()
{
	double x_min = 1e20, x_max = -1e20;
	double y_min = 1e20, y_max = -1e20;
	double z_min = 1e20, z_max = -1e20;

	//вычисляем максимум и минимиум покоординатно
	for (int i = 0; i < (int) points_.size(); ++i)
	{
		double x = points_[i].x;
		double y = points_[i].y;
		double z = points_[i].z;

		if (x > x_max) x_max = x;
		if (x < x_min) x_min = x;

		if (y > y_max) y_max = y;
		if (y < y_min) y_min = y;

		if (z > z_max) z_max = z;
		if (z < z_min) z_min = z;
	}

	if (x_max == x_min) {
		x_max = 1;
		x_min = -1;
	}

	if (y_max == y_min) {
		y_max = 1;
		y_min = -1;
	}

	if (z_max == z_min) {
		z_max = 1;
		z_min = -1;
	}

	double xx2 = 2.0 / (x_max - x_min);
	double yy2 = 2.0 / (y_max - y_min);
	double zz2 = 2.0 / (z_max - z_min);

	double k = zz2;
	if (xx2 <= yy2 && xx2 <= zz2) {
		k = xx2;
	} else if (yy2 <= xx2 && yy2 <= zz2) {
		k = yy2;
	} else if (zz2 <= xx2 && zz2 <= yy2) {
		k = zz2;
	}

	for (int i = 0; i < (int) points_.size(); ++i)
	{
		double & x = points_[i].x;
		double & y = points_[i].y;
		double & z = points_[i].z;

		x = (x - x_min) * k - 1.0;
		y = (y - y_min) * k - 1.0;
		z = (z - z_min) * k - 1.0;
	}
}

void Viz_Triang::load_file()
{
	FILE * f = fopen (fname_.c_str(), "r");
	int size;
	int lineno = 0;

#ifdef _DEBUG
	if (!f) throw NotFound (fname_.c_str(), __FILE__, __LINE__);
#else
	if (!f) throw NotFound (fname_.c_str() );
#endif

#define _BUF_SZ 32768
	char s[32768];

	fgets (s, _BUF_SZ - 1, f); lineno ++;
	do
	{
		if (*s != '#')
			break;

		lineno ++;
	}
	while (fgets (s, _BUF_SZ - 1, f) );

	do
	{
		double x = 0.0, y = 0.0, z = 0.0; // global coordinated
		double u = 0.0, v = 0.0;          // local coordinates
		double f = 0.0;
		int m;

		if (*s == '#')
			break;

		m = sscanf (s, "%lf%lf%lf%lf%lf%lf", &x, &y, &z, &f, &u, &v);
		if (m < 2)
		{
			goto bad;
		}

		points_.push_back (Viz_Point (x, y, z, f, u, v));
		lineno ++;
	}
	while (fgets (s, _BUF_SZ - 1, f) );

	size = (int) points_.size();

	adj_.resize(size);

	fgets (s, _BUF_SZ - 1, f); lineno ++;
	do
	{
		int n1, n2, n3;

		if (*s == '#') {
			break;
		}

		if (sscanf (s, "%d%d%d", &n1, &n2, &n3) != 3)
		{
			goto bad;
		}

		//так как индексы в файле с 1 а не с 0
		--n1;
		--n2;
		--n3;
		if (n1 >= size || n2 >= size || n3 >= size)
		{
			goto bad;
		}

		vector < int > triplet (3);
		triplet[0] = n1;
		triplet[1] = n2;
		triplet[2] = n3;

		nodes_.push_back (triplet);
		lineno ++;

		adj_[n1].push_back(n2); adj_[n1].push_back(n3);
		adj_[n2].push_back(n1); adj_[n2].push_back(n3);
		adj_[n3].push_back(n1); adj_[n3].push_back(n2);
	}
	while (fgets (s, _BUF_SZ - 1, f) );

	return;

bad:
	{
		ostringstream ss;
		ss << "bad file format, lineno: " << lineno;
		string out = ss.str();
#ifdef _DEBUG
		throw BadArgument (out.c_str(), __FILE__, __LINE__);
#else
		throw BadArgument (out.c_str());
#endif
	}
}

void Viz_Triang::draw()
{
	if (hidden_)
		return;

	int sz = (int) nodes_.size();

	glPushMatrix();

	switch (dMode_)
	{
	case dPoints:
		glBegin (GL_POINTS);
		glColor3d (0.0, 0.0, 1.0);

		sz = (int) points_.size();

		for (int i = 0; i < sz; ++i)
		{
			Viz_Point & p1 = points_[i];

			glColor3d (0.0, 0.0, p1.z);
			glVertex3d (p1.x, p1.y, p1.z);
		}

		glEnd();
		break;

	case dLines:
		glEnable (GL_POLYGON_OFFSET_FILL);
		glPolygonOffset (1, 1);

		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		glCallList (fill_);
		glDisable (GL_POLYGON_OFFSET_FILL);

	case dFill:
		if (dMode_ == dFill)
		{
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			glCallList (fill_);
		}
		else
		{
			glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
			glColor4d (1.0, 0.0, 0.0, 1.0);
			glCallList (wire_);
		}
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		break;
	case dIsolines:
		break;
	default:
		glPopMatrix();
		glFlush();
		throw BadArgument ("unknown drawing mode", __FILE__, __LINE__);
		break;
	}

	glPopMatrix();
	glFlush();
}

static int arg_min_f(std::vector < Viz_Point > & points, double C)
{
	int m = 0;
	for (int i = 0; i < (int)points.size(); ++i) {
		if (fabs(points[i].f - C) < fabs(points[m].f - C)) {
			m = i;
		}
	}
	return m;
}

static int arg_min_f2(std::vector < Viz_Point > & points, std::vector < int > & adj, int root, double C)
{
	int m = adj[0];
	for (int i = 0; i < (int)adj.size(); ++i) {
		int j = adj[i];
		if (j == root) continue;
		if (fabs(points[j].f - C) < fabs(points[m].f - C)) {
			m = i;
		}
	}
	return m;
}

void Viz_Triang::build_isolines()
{
	int nlines   = 4; 
	double u_min = 1e20, u_max = -1e20;
	double v_min = 1e20, v_max = -1e20;
	double f_min = 1e20, f_max = -1e20;

	//вычисляем максимум и минимиум покоординатно
	for (int i = 0; i < (int) points_.size(); ++i)
	{
		double u = points_[i].u;
		double v = points_[i].v;
		double f = points_[i].f;

		if (u > u_max) u_max = u;
		if (u < u_min) u_min = u;

		if (v > v_max) v_max = v;
		if (v < v_min) v_min = v;

		if (f > f_max) f_max = f;
		if (f < f_min) f_min = f;
	}

	if (   u_max == u_min
		|| v_max == v_min
		|| f_max == f_min)
	{
		//TODO: show status
		return;
	}

	vector < vector < int > > isolines;
	isolines.reserve(nlines + 1);

	for (int i = 0; i <= nlines; ++i) {
		double C = f_min + (double)i * (f_max - f_min) / nlines;
		vector < int > isoline;
		set < int > ps;

		//1. ищем аргумент(ы) min |F(u, v) - C|
		int m = arg_min_f(points_, C); isoline.push_back(m); ps.insert(m);
		//2. для перехода к следующей точки рассматриваем все смежные с (u,v) узлы
		while (true) {
			//3. ищем аргумент min|F(u1, v1) - C|, где (u1, v1) смежный узел с (u, v)
			m = arg_min_f2(points_, adj_[m], m, C);
			//4. если (u1,v1) уже было рассмотрено ранее, то выходим, иначе переходим к (3)
			if (ps.find(m) != ps.end()) {
				break;
			}
			isoline.push_back(m); ps.insert(m);
		}

		isolines.push_back(isoline);
	}

	fprintf(stderr, "isolines done\n");
}

void Viz_Triang::gen_lists()
{
	int sz = (int) nodes_.size();
	wire_ = glGenLists (1);
	fill_ = glGenLists (1);

	glNewList (wire_, GL_COMPILE);
	glBegin (GL_TRIANGLES);
	for (int i = 0; i < sz; ++i)
	{
		Viz_Point & p1 = points_[nodes_[i][0]];
		Viz_Point & p2 = points_[nodes_[i][1]];
		Viz_Point & p3 = points_[nodes_[i][2]];

		glVertex3d (p1.x, p1.y, p1.z);
		glVertex3d (p2.x, p2.y, p2.z);
		glVertex3d (p3.x, p3.y, p3.z);
	}
	glEnd();
	glEndList();

	glNewList (fill_, GL_COMPILE);
	glBegin (GL_TRIANGLES);
	for (int i = 0; i < sz; ++i)
	{
		Viz_Point & p1 = points_[nodes_[i][0]];
		Viz_Point & p2 = points_[nodes_[i][1]];
		Viz_Point & p3 = points_[nodes_[i][2]];

		glColor3d (0.0, 0.0, p1.f);
		glVertex3d (p1.x, p1.y, p1.z);

		glColor3d (0.0, 0.0, p2.f);
		glVertex3d (p2.x, p2.y, p2.z);

		glColor3d (0.0, 0.0, p3.f);
		glVertex3d (p3.x, p3.y, p3.z);
	}
	glEnd();
	glEndList();
}

void Viz_Triang::keyPressEvent1 ( unsigned char key, int x, int y )
{
	switch (key)
	{
	case 'd':
		dMode_ = (dMode_ + 1) % 4;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

