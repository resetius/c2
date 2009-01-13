#ifndef _VIZ_SPHERE_H
#define _VIZ_SPHERE_H
/*$Id: viz_sphere.h 1867 2008-05-23 21:11:01Z manwe $*/

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
#include <map>
#include "viz_surface.h"

class Viz_Sphere: public Viz_Surface {
public:
	enum {
		sHalf = 0, //!<полусфера
		sFull = 1, //!<полная сфера
		sFlat = 2  //!<в виде круга
	};

	Viz_Sphere(const char * ctrl_points_file, int sType = sHalf);
	~Viz_Sphere();

	void draw();

private:
	int zones; //!<количество цветовых зон
	std::map < int, GLfloat * > m; //!<цветовая карта
	int color(float); //!<цветовой индекс по значению

	int n_phi;
	int n_la;

	int type;

	GLuint sphere;

	double d_la;
	double d_phi;

	float min;
	float max;

	void init();
	void init_sphere();
};

#endif //SPHERE
