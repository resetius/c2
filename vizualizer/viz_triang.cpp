/*$Id: viz_triang.cpp 1640 2007-02-23 20:43:20Z manwe $*/

/* Copyright (c) 2007 Alexey Ozeritsky
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
#include <cstdio>
#include <iostream>
#include <sstream>
#include "asp_excs.h"
#include "asp_interpolate.h"

#include "viz_triang.h"

using namespace std;

Viz_Triang::Viz_Triang(const char * file)
	: fname_(file)
{
	load_file();
	normalize();

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

    //��������� �������� � �������� �������������
	for (int i = 0; i < (int)points_.size(); ++i) {
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

    double xx2 = 2.0 / (x_max - x_min);
    double yy2 = 2.0 / (y_max - y_min);
    double zz2 = 2.0 / (z_max - z_min);

	for (int i = 0; i < (int)points_.size(); ++i) {
		double & x = points_[i].x;
		double & y = points_[i].y;
		double & z = points_[i].z;

		x = (x - x_min) * xx2 - 1.0;
		y = (y - y_min) * yy2 - 1.0;
		z = (z - z_min) * zz2 - 1.0;
	}
}

void Viz_Triang::load_file()
{
	FILE * f = fopen(fname_.c_str(), "r");
	int size;
#ifdef _DEBUG
	if (!f) throw NotFound(fname_.c_str(), __FILE__, __LINE__);
#else
	if (!f) throw NotFound(fname_.c_str());
#endif

#define _BUF_SZ 32768
	char s[32768];
	
	fgets(s, _BUF_SZ - 1, f);

	do {
		double x, y, z;

		if (*s == '#')
			break;

		if (sscanf(s, "%lf%lf%lf", &x, &y, &z) != 3) {
			goto bad;
		}

		points_.push_back(Viz_Point(x, y, z));
	} while (fgets(s, _BUF_SZ - 1, f));

	size = (int)points_.size();

	fgets(s, _BUF_SZ - 1, f);
	do {
		int n1, n2, n3;
		if (sscanf(s, "%d%d%d", &n1, &n2, &n3) != 3) {
			goto bad;
		}

		//��� ��� ������� � ����� � 1 � �� � 0
		--n1; --n2; --n3;
		if (n1 >= size || n2 >= size || n3 >= size) {
			goto bad;
		}

		vector < int > triplet(3);
		triplet[0] = n1;
		triplet[1] = n2;
		triplet[2] = n3;

		nodes_.push_back(triplet);

	} while (fgets(s, _BUF_SZ - 1, f));

	return;

bad:
	{
		string out = "bad file format";
#ifdef _DEBUG
		throw BadArgument(out.c_str(), __FILE__, __LINE__);
#else
		throw BadArgument(out.c_str());
#endif
	}
}

void Viz_Triang::draw()
{
	if (hidden_)
		return;

	int sz = (int)nodes_.size();

	glPushMatrix();

	switch (dMode_) {
	case dPoints:
        glBegin(GL_POINTS);
		glColor3d(0.0, 0.0, 1.0);

		sz = (int)points_.size();

		for (int i = 0; i < sz; ++i) {
			Viz_Point & p1 = points_[i];

			glColor3d(0.0, 0.0, p1.z);
			glVertex3d(p1.x, p1.y, p1.z);
		}

		glEnd();
		break;

    case dLines:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    case dFill:
        if (dMode_ == dLines) {
			;
        } else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

		glBegin(GL_TRIANGLES);

		//glColor3d(0.0, 0.0, 1.0);
		for (int i = 0; i < sz; ++i) {
			Viz_Point & p1 = points_[nodes_[i][0]];
			Viz_Point & p2 = points_[nodes_[i][1]];
			Viz_Point & p3 = points_[nodes_[i][2]];
			
			glColor3d(0.0, 0.0, p1.z);
			glVertex3d(p1.x, p1.y, p1.z);

			glColor3d(0.0, 0.0, p2.z);
			glVertex3d(p2.x, p2.y, p2.z);

			glColor3d(0.0, 0.0, p3.z);
			glVertex3d(p3.x, p3.y, p3.z);
		}

		glEnd();
		break;

    default:
        throw BadArgument("unknown drawing mode", __FILE__, __LINE__);
        break;
	}

	glPopMatrix();
	glFlush();
}

void Viz_Triang::keyPressEvent1 ( unsigned char key, int x, int y ) {
    switch(key) {
    case 'd':
        setDrawMode((DrawMode)(((int)dMode_ + 1) % 3));
        glutPostRedisplay();
        break;
    default:
        break;
    }
}