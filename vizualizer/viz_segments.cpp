/*$Id$*/

/* Copyright (c) 2008 Alexey Ozeritsky
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

#include <stdio.h>
#include "viz_segments.h"

using namespace std;

static GLuint pal[][3] = {
	{0, 0, 0},
	{0, 0, 168},
	{0, 168, 0},
	{0, 168, 168},
	{168, 0, 0},
	{168, 0, 168},
	{168, 84, 0},
	{168, 168, 168},
	{84, 84, 84},
	{84, 84, 252},
	{84, 252, 84},
	{84, 252, 252},
	{252, 84, 84},
	{252, 84, 252},
	{252, 252, 84},
	{252, 252, 252},
	{0, 0, 0},
	{20, 20, 20},
	{32, 32, 32},
	{44, 44, 44},
	{56, 56, 56},
	{68, 68, 68},
	{80, 80, 80},
	{96, 96, 96},
	{112, 112, 112},
	{128, 128, 128},
	{144, 144, 144},
	{160, 160, 160},
	{180, 180, 180},
	{200, 200, 200},
	{224, 224, 224},
	{252, 252, 252},
	{0, 0, 252},
	{64, 0, 252},
	{124, 0, 252},
	{188, 0, 252},
	{252, 0, 252},
	{252, 0, 188},
	{252, 0, 124},
	{252, 0, 64},
	{252, 0, 0},
	{252, 64, 0},
	{252, 124, 0},
	{252, 188, 0},
	{252, 252, 0},
	{188, 252, 0},
	{124, 252, 0},
	{64, 252, 0},
	{0, 252, 0},
	{0, 252, 64},
	{0, 252, 124},
	{0, 252, 188},
	{0, 252, 252},
	{0, 188, 252},
	{0, 124, 252},
	{0, 64, 252},
	{124, 124, 252},
	{156, 124, 252},
	{188, 124, 252},
	{220, 124, 252},
	{252, 124, 252},
	{252, 124, 220},
	{252, 124, 188},
	{252, 124, 156},
	{252, 124, 124},
	{252, 156, 124},
	{252, 188, 124},
	{252, 220, 124},
	{252, 252, 124},
	{220, 252, 124},
	{188, 252, 124},
	{156, 252, 124},
	{124, 252, 124},
	{124, 252, 156},
	{124, 252, 188},
	{124, 252, 220},
	{124, 252, 252},
	{124, 220, 252},
	{124, 188, 252},
	{124, 156, 252},
	{180, 180, 252},
	{196, 180, 252},
	{216, 180, 252},
	{232, 180, 252},
	{252, 180, 252},
	{252, 180, 232},
	{252, 180, 216},
	{252, 180, 196},
	{252, 180, 180},
	{252, 196, 180},
	{252, 216, 180},
	{252, 232, 180},
	{252, 252, 180},
	{232, 252, 180},
	{216, 252, 180},
	{196, 252, 180},
	{180, 252, 180},
	{180, 252, 196},
	{180, 252, 216},
	{180, 252, 232},
	{180, 252, 252},
	{180, 232, 252},
	{180, 216, 252},
	{180, 196, 252},
	{0, 0, 112},
	{28, 0, 112},
	{56, 0, 112},
	{84, 0, 112},
	{112, 0, 112},
	{112, 0, 84},
	{112, 0, 56},
	{112, 0, 28},
	{112, 0, 0},
	{112, 28, 0},
	{112, 56, 0},
	{112, 84, 0},
	{112, 112, 0},
	{84, 112, 0},
	{56, 112, 0},
	{28, 112, 0},
	{0, 112, 0},
	{0, 112, 28},
	{0, 112, 56},
	{0, 112, 84},
	{0, 112, 112},
	{0, 84, 112},
	{0, 56, 112},
	{0, 28, 112},
	{56, 56, 112},
	{68, 56, 112},
	{84, 56, 112},
	{96, 56, 112},
	{112, 56, 112},
	{112, 56, 96},
	{112, 56, 84},
	{112, 56, 68},
	{112, 56, 56},
	{112, 68, 56},
	{112, 84, 56},
	{112, 96, 56},
	{112, 112, 56},
	{96, 112, 56},
	{84, 112, 56},
	{68, 112, 56},
	{56, 112, 56},
	{56, 112, 68},
	{56, 112, 84},
	{56, 112, 96},
	{56, 112, 112},
	{56, 96, 112},
	{56, 84, 112},
	{56, 68, 112},
	{80, 80, 112},
	{88, 80, 112},
	{96, 80, 112},
	{104, 80, 112},
	{112, 80, 112},
	{112, 80, 104},
	{112, 80, 96},
	{112, 80, 88},
	{112, 80, 80},
	{112, 88, 80},
	{112, 96, 80},
	{112, 104, 80},
	{112, 112, 80},
	{104, 112, 80},
	{96, 112, 80},
	{88, 112, 80},
	{80, 112, 80},
	{80, 112, 88},
	{80, 112, 96},
	{80, 112, 104},
	{80, 112, 112},
	{80, 104, 112},
	{80, 96, 112},
	{80, 88, 112},
	{0, 0, 64},
	{16, 0, 64},
	{32, 0, 64},
	{48, 0, 64},
	{64, 0, 64},
	{64, 0, 48},
	{64, 0, 32},
	{64, 0, 16},
	{64, 0, 0},
	{64, 16, 0},
	{64, 32, 0},
	{64, 48, 0},
	{64, 64, 0},
	{48, 64, 0},
	{32, 64, 0},
	{16, 64, 0},
	{0, 64, 0},
	{0, 64, 16},
	{0, 64, 32},
	{0, 64, 48},
	{0, 64, 64},
	{0, 48, 64},
	{0, 32, 64},
	{0, 16, 64},
	{32, 32, 64},
	{40, 32, 64},
	{48, 32, 64},
	{56, 32, 64},
	{64, 32, 64},
	{64, 32, 56},
	{64, 32, 48},
	{64, 32, 40},
	{64, 32, 32},
	{64, 40, 32},
	{64, 48, 32},
	{64, 56, 32},
	{64, 64, 32},
	{56, 64, 32},
	{48, 64, 32},
	{40, 64, 32},
	{32, 64, 32},
	{32, 64, 40},
	{32, 64, 48},
	{32, 64, 56},
	{32, 64, 64},
	{32, 56, 64},
	{32, 48, 64},
	{32, 40, 64},
	{44, 44, 64},
	{48, 44, 64},
	{52, 44, 64},
	{60, 44, 64},
	{64, 44, 64},
	{64, 44, 60},
	{64, 44, 52},
	{64, 44, 48},
	{64, 44, 44},
	{64, 48, 44},
	{64, 52, 44},
	{64, 60, 44},
	{64, 64, 44},
	{60, 64, 44},
	{52, 64, 44},
	{48, 64, 44},
	{44, 64, 44},
	{44, 64, 48},
	{44, 64, 52},
	{44, 64, 60},
	{44, 64, 64},
	{44, 60, 64},
	{44, 52, 64},
	{44, 48, 64},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
};

Viz_Segments::Viz_Segments(const char * fname)
{
	oType_  = Viz_Obj::oGeneral;
	dMode_  = Viz_Obj::dLines;
	hidden_ = false;
	lst     = -1;

	FILE * f = fopen(fname, "r");
	if (!f) return;

	line l;
	while (fscanf(f, "{%lf, %lf}-{%lf, %lf} %d\n", 
		&l.p1.x, &l.p1.y,
		&l.p2.x, &l.p2.y, &l.c) == 5)
	{
		segments_.push_back(l);
	}

	fclose(f);

	gen_list();
}

Viz_Segments::~Viz_Segments()
{
}

void Viz_Segments::draw()
{
	if (hidden_)
		return;

	if (lst > 0) {
		glPushMatrix();
		glCallList(lst);
		glPopMatrix();
		glFlush();
	}
}

void Viz_Segments::gen_list() 
{
	lst = glGenLists(1);
	GLubyte col = 0;
	GLubyte r, g, b;
	GLubyte pr, pg, pb;

	glNewList(lst, GL_COMPILE);
	glBegin(GL_LINES);
	point p;

	for (std::list < line >::iterator it = segments_.begin();
		it != segments_.end(); ++it)
	{
		col = (GLbyte)it->c;
		r = pal[col][0];
		g = pal[col][1];
		b = pal[col][2];

		//b   = (col >> 0) & 3; b *= (255 / 4);
		//g   = (col >> 2) & 7; g *= (255 / 8);
		//r   = (col >> 5) & 7; r *= (255 / 8);

		if (p != it->p1)
			glColor3ub(r, g, b);
		else
			glColor3ub(pr, pg, pb);

		glVertex3d(it->p1.x, it->p1.y, 0.0);

		glColor3ub(r, g, b);
		glVertex3d(it->p2.x, it->p2.y, 0.0);

		p = it->p2;
		pr = r; pb = b; pg = g;
	}
	glEnd();
	glEndList();
}

