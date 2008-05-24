#ifndef _VIZ_SURFACE_H
#define _VIZ_SURFACE_H
/*$Id: viz_surface.h 1867 2008-05-23 21:11:01Z manwe $*/

/* Copyright (c) 2005 Alexey Ozeritsky
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

#include "viz_obj.h"

class Viz_Surface: public Viz_Obj {
protected:
	GLfloat * ctrl_points;
	GLfloat * colors;
    GLfloat * knots1;
    GLfloat * knots2;
	double * data;
	int rows; //размеры матрицы ctrl_points
	int cols;
    GLUnurbsObj *theNurb;

public:
    enum SurfaceType {
        sPlain = 0, //точки соединяются линиями
        sNurbs = 1  //используются средства OpenGL для построения NURBS поверхности по точкам
    };

	//flags - флаги
	// 1 - не делать нормализацию
	Viz_Surface(const char * ctrl_points_file, int sType = sPlain, int mode = dLines,
		int flags = 0);
	virtual ~Viz_Surface();

	void draw();
    int getSurfaceType();

    void keyPressEvent1 ( unsigned char key, int x, int y );
    bool trySelect(int x, int y);
    bool selected();
	void select();

private:
    bool sel;
    int sType;
	int flags;

    void init();
	void init_nurbs();
    void init_plain();
    void draw_nurbs();
    void draw_plain();

    void normalize_data();  //загоняет данные в единичный квадрат

	void load_ctrl_points_from_matrix_file(const char * ctrl_points_file);
    void searchBoundingRect();
};

#endif //_VIZ_SURFACE_H
