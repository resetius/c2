/*$Id: viz_surface.cpp 1867 2008-05-23 21:11:01Z manwe $*/

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
/**
 * рисует NURBS поверхность, заданную контрольными точками
 * из файла или из матрицы
 */

#ifdef _WIN32
#define GLUT_BUILDING_LIB
#endif
#include <GL/glut.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include "asp_excs.h"
#include "viz_surface.h"

using namespace std;

Viz_Surface::~Viz_Surface()
{
	if (ctrl_points) delete [] ctrl_points;
	if (colors     ) delete [] colors;
    if (knots1     ) delete [] knots1;
    if (knots2     ) delete [] knots2;
	if (data       ) delete [] data;
}

Viz_Surface::Viz_Surface(const char * ctrl_points_file, int sType1, int mode, int _flags)
{
	flags       = _flags;
	oType_      = oGeneral | oSurface;
	ctrl_points = 0;
	colors      = 0;
    knots1      = 0;
    knots2      = 0;
	data        = 0;
    sel         = false;
	load_ctrl_points_from_matrix_file(ctrl_points_file);
//	printf("ctrl_points;\n");
//	printfvector(ctrl_points, rows * cols * 3, rows * cols * 3, " %.2le");
//	printfwmatrix(ctrl_points, rows, cols, 8, " %.2lf");
//  double * c = ctrl_points;
//  for (int i = 0; i < rows * cols * 3; i += 3) {
//	   printf("{%.2le, %.2le, %.2le}\n", c[i], c[i + 1], c[i + 2]);
//  }  
    sType = sType1;
    dMode_= mode;
	init();
}

bool Viz_Surface::selected() {
    return sel;
}

bool Viz_Surface::trySelect (int x, int y) {
    if (frame_ == 0) {
        return false;
    }

    if (frame_->isInside(x, y)) {
		select();
        return true;
    }
    return false;
}

void Viz_Surface::select() {
	double t;
	if (selected()) {
		sel = false;
	} else {
		sel = true;
	}

	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			int offset = i * rows * 3 + j * 3;
			offset = i * rows * 4 + j * 4;
			colors[offset    ] = 0.0;
			t = colors[offset + 1]; colors[offset + 1] = colors[offset + 2];
			colors[offset + 2] = t;
			colors[offset + 3] = 1.0;			
		}
	}
	glutPostRedisplay();
}

/**
 * матричный формат файла
 * a11 ..... a1n
 * .............
 * an1 ..... ann
 */
void Viz_Surface::load_ctrl_points_from_matrix_file(const char * ctrl_points_file)
{
	FILE * f = fopen(ctrl_points_file, "r");
#ifdef _DEBUG
	if (!f) throw NotFound(ctrl_points_file, __FILE__, __LINE__);
#else
    if (!f) throw NotFound(ctrl_points_file);
#endif
	vector < vector < float > * > pre_matrix;
	vector < float > * row;
	float item;
	const int buf_size = 32768;
	char buf [buf_size];
	istringstream *input;
	
	cols = 0; 
	//читаем первую строку для определения cols
	row = new vector < float >;
	if (fgets(buf, buf_size - 1, f) == 0) {
        string out = string(ctrl_points_file) + ": bad file format";
#ifdef _DEBUG
        throw BadArgument(out.c_str(), __FILE__, __LINE__);
#else
        throw BadArgument(out.c_str());
#endif
	}
	
	input = new istringstream(buf);
	while (input->good()) {
		*input >> item; if (!input->good()) break;
		cols ++;
		row->push_back(item);
	}
	rows = 1;
	pre_matrix.push_back(row);
	while (fgets(buf, buf_size - 1, f) != 0) {
        bool good_str = true;
		row = new vector < float >;
		delete input; input = new istringstream(buf);
		for (int i = 0; i < cols; i++) {
			*input >> item;
			if (!input->good()) {
                //неоконченная строка. пропускаем
                good_str = false;
                delete row;
                break;
			}
			row->push_back(item);
		}
        if (good_str) {
		    pre_matrix.push_back(row);
		    rows ++;
        }
	}
	ctrl_points = new float[rows * cols * 3];
	colors      = new float[rows * cols * 4];
	data        = new double[rows * cols];
	
	float min = 1e20, max = -1e20;
	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			float t = pre_matrix.at(j)->at(i);
			if (t > max) max = t;
			if (t < min) min = t;
		}
	}
	
	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			int offset = i * rows * 3 + j * 3;
			ctrl_points[offset    ] = i;
			ctrl_points[offset + 1] = j;
			ctrl_points[offset + 2] = pre_matrix.at(j)->at(i);
			data[j * cols + i] = pre_matrix.at(j)->at(i);
			
			offset = i * rows * 4 + j * 4;
			colors[offset    ] = 0.0;
			colors[offset + 1] = 0.0;
			colors[offset + 2] = (pre_matrix.at(j)->at(i) - min) /(max - min);
			colors[offset + 3] = 1.0;
		}
	}

	if (!(flags & 1))
		normalize_data();

	for (int j = 0; j < rows; j++) {
		delete pre_matrix.at(j);
	}
	delete input;
	fclose(f);
}

int Viz_Surface::getSurfaceType() {
    return sType;
}

void Viz_Surface::draw() {
    switch (sType) {
    case sPlain:
        draw_plain();
        break;
    case sNurbs:
        draw_nurbs();
        break;
    default:
        throw BadArgument("unknown surface type", __FILE__, __LINE__);
    }
    searchBoundingRect();
    //cout << "x,y w,h: " << frame->x() << "," << frame->y() <<" " << frame->width() << "," 
    //    << frame->height() << endl;
}

void Viz_Surface::searchBoundingRect() {
    float *p = ctrl_points;
    GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    double x, y, z;
    double max_x = -1e20, min_x = 1e20;
    double max_y = -1e20, min_y = 1e20;


    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            int offset  = i * rows * 3 + j * 3;
			gluProject(p[offset], p[offset + 1], p[offset + 2], mvmatrix, projmatrix, viewport,
				&x, &y, &z);
            if (x > max_x) max_x = x;
            if (x < min_x) min_x = x;

            if (y > max_x) max_y = y;
            if (y < min_x) min_y = y;
        }
    }
    if (frame_ == 0) {
        frame_ = new Rect((int)min_x, (int)min_y, (int)(max_x - min_x), (int)(max_y - min_y));
    } else {
        frame_->move((int)min_x, (int)min_y);
        frame_->resize((int)(max_x - min_x), (int)(max_y - min_y));
    }
}

void Viz_Surface::draw_plain() {
    float *p = ctrl_points;
	float *c = colors;
    int offset = 0;
	int coffset = 0;

    glPushMatrix();

    switch (dMode_) {
    case dPoints:
        glBegin(GL_POINTS);
        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                offset  = i * rows * 3 + j * 3;
				coffset = i * rows * 4 + j * 4;
				glColor4f(c[coffset], c[coffset + 1], c[coffset + 2], c[coffset + 3]);
				glVertex3d(p[offset], p[offset + 1], p[offset + 2]);
            }
        }
        glEnd();
        break;
    case dLines:
    case dFill:
        if (dMode_ == dLines) {
            glBegin(GL_LINE_STRIP);
        } else {
            glBegin(GL_TRIANGLE_STRIP);
        }
        for (int i = 0; i < cols - 1; i += 1) {
            for (int j = 0; j < rows - 1; j += 2) {
                offset  = (i    ) * rows * 3 + (j    ) * 3;
				coffset = (i    ) * rows * 4 + (j    ) * 4;
				glColor4f(c[coffset], c[coffset + 1], c[coffset + 2], c[coffset + 3]);
                glVertex3d(p[offset], p[offset + 1], p[offset + 2]);

                offset  = (i + 1) * rows * 3 + (j    ) * 3;
				coffset = (i + 1) * rows * 4 + (j    ) * 4;
				glColor4f(c[coffset], c[coffset + 1], c[coffset + 2], c[coffset + 3]);
                glVertex3d(p[offset], p[offset + 1], p[offset + 2]);

                offset  = (i    ) * rows * 3 + (j + 1) * 3;
				coffset = (i    ) * rows * 4 + (j + 1) * 4;
				glColor4f(c[coffset], c[coffset + 1], c[coffset + 2], c[coffset + 3]);
                glVertex3d(p[offset], p[offset + 1], p[offset + 2]);

                offset  = (i + 1) * rows * 3 + (j + 1) * 3;
				coffset = (i + 1) * rows * 4 + (j + 1) * 4;
				glColor4f(c[coffset], c[coffset + 1], c[coffset + 2], c[coffset + 3]);
                glVertex3d(p[offset], p[offset + 1], p[offset + 2]);
            }
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

void Viz_Surface::draw_nurbs() {
	glPushMatrix();

    gluBeginSurface(theNurb);
    gluNurbsSurface(theNurb, cols * 2, knots1, rows * 2, knots2, cols * 3, 3,
                      &ctrl_points[0], 4, 4,GL_MAP2_VERTEX_3);
    gluEndSurface(theNurb);
    glPopMatrix();
	glFlush();

}

void Viz_Surface::init() {
    switch (sType) {
    case sPlain:
        init_plain();
        break;
    case sNurbs:
        init_nurbs();
        break;
    default:
        throw BadArgument("unknown surface type", __FILE__, __LINE__);
    }
}

void Viz_Surface::init_plain() {
    //!todo\ сделать инициализанию списка отображения
}

//void CALLBACK nurbsError(GLenum errorCode) {
//	char message[100];
//
//	//sprintf(message,"NURBS error: %s\n",gluErrorString(errorCode));
//	//MessageBox(NULL,message,"NURBS surface",0);
//	exit(0);
//}

void Viz_Surface::init_nurbs() {
    knots1 = new GLfloat[cols * 2];
    knots2 = new GLfloat[rows * 2];

    for (int i = 0; i < cols * 2; i++) {
        knots1[i] = i / 2;
    }

    for (int i = 0; i < rows * 2; i++) {
        knots2[i] = i / 2;
    }

    theNurb = gluNewNurbsRenderer();
    gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 2.0);
    gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
    //gluNurbsCallback(theNurb,GLU_ERROR,(void(__stdcall *)(void))nurbsError);
}

void Viz_Surface::normalize_data() {
    float x_min = 1e20, x_max = -1e20;
    float y_min = 1e20, y_max = -1e20;
    float z_min = 1e20, z_max = -1e20;

    //вычисляем максимум и минимиум покоординатно
	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			int offset = i * rows * 3 + j * 3;
            float x = ctrl_points[offset    ];
            float y = ctrl_points[offset + 1];
            float z = ctrl_points[offset + 2];

            if (x > x_max) x_max = x;
            if (x < x_min) x_min = x;

            if (y > y_max) y_max = y;
            if (y < y_min) y_min = y;

            if (z > z_max) z_max = z;
            if (z < z_min) z_min = z;
        }
    }

    float xx2 = 2.0 / (x_max - x_min);
    float yy2 = 2.0 / (y_max - y_min);
    float zz2 = 2.0 / (z_max - z_min);
    
    //нормализуем
	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			int offset = i * rows * 3 + j * 3;
            float &x = ctrl_points[offset    ];
            float &y = ctrl_points[offset + 1];
            float &z = ctrl_points[offset + 2];

            x = (x - x_min) * xx2 - 1.0;
            y = (y - y_min) * yy2 - 1.0;
            z = (z - z_min) * zz2 - 1.0;

			data[j * cols + i] = z;
        }
    }
}

void Viz_Surface::keyPressEvent1 ( unsigned char key, int x, int y ) {
    switch(key) {
    case 'd':
        setDrawMode((DrawMode)(((int)dMode_ + 1) % 3));
        glutPostRedisplay();
        break;
    default:
        break;
    }
}
