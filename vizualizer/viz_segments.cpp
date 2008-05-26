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

Viz_Segments::Viz_Segments(const char * fname)
{
	oType_  = Viz_Obj::oGeneral;
	dMode_  = Viz_Obj::dLines;
	hidden_ = false;
	lst     = -1;

	FILE * f = fopen(fname, "r");
	if (!f) return;

	line l;
	while (fscanf(f, "{%lf, %lf}-{%lf, %lf}\n", 
		&l.p1.x, &l.p1.y,
		&l.p2.x, &l.p2.y) == 4)
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

	glNewList(lst, GL_COMPILE);
	glBegin(GL_LINES);
	for (std::list < line >::iterator it = segments_.begin();
		it != segments_.end(); ++it)
	{
		glColor4f(1.0, 0.0, 0.0, 1.0);
		glVertex3d(it->p1.x, it->p1.y, 0.0);
		glColor4f(1.0, 0.0, 0.0, 1.0);
		glVertex3d(it->p2.x, it->p2.y, 0.0);
	}
	glEnd();
	glEndList();
}

