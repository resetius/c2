/*$Id: viz_mainwindow.cpp 407 2005-10-15 10:32:15Z manwe $*/

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
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <time.h>
#include <iostream>
#include "asp_excs.h"
#include "viz_obj.h"
#include "viz_mainwindow.h"
#include "viz_surface.h"

using namespace std;

VizMainWindow * mainwindow = 0;

/**
 * глобальные функции - врапперы для
 * glut*Func* вызовов
 */
/*__________________________________*/
void global_timer (int value)
{
	static int step = 1;
	if (mainwindow != 0)
	{
		mainwindow->timer (value);
	}
	step ++;
	glutTimerFunc (1000, global_timer, step);
}

void global_repaint()
{
	if (mainwindow != 0)
	{
		mainwindow->repaint();
	}
}

void global_idle()
{
	if (mainwindow != 0)
	{
		mainwindow->idle();
	}
}

void global_resize (int w, int h)
{
	if (mainwindow != 0)
	{
		mainwindow->resize (w, h);
	}
}

void global_mouseMoveEvent ( int x, int y )
{
	if (mainwindow != 0)
	{
		mainwindow->mouseMoveEvent (x, y);
	}
}

void global_mousePressEvent ( int button, int state, int x, int y )
{
	if (mainwindow != 0)
	{
		mainwindow->mousePressEvent (button, state, x, y);
	}
}

void global_keyPressEvent1 ( unsigned char key, int x, int y )
{
	if (mainwindow != 0)
	{
		mainwindow->keyPressEvent1 (key, x, y);
	}
}

void global_keyPressEvent2 ( int key, int x, int y )
{
	if (mainwindow != 0)
	{
		mainwindow->keyPressEvent2 (key, x, y);
	}
}

void global_fileMenu (int i)
{
	if (mainwindow != 0)
	{
		mainwindow->fileMenu (i);
	}
}

void global_actionsMenu (int i)
{
	if (mainwindow != 0)
	{
		mainwindow->actionsMenu (i);
	}
}

void global_optionsMenu (int i)
{
	if (mainwindow != 0)
	{
		mainwindow->optionsMenu (i);
	}
}
/*___________________________________________________________________*/
VizMainWindow::~VizMainWindow()
{
	delete v_objs;
	delete p;
}

VizMainWindow::VizMainWindow (const char *name1, int w, int h, int argc, char **argv)
		: p (new Config ("vizualizer.ini", argc, argv) ), name (name1), frame (new Rect (w, h) )
{
	old_mouse_y = -1;
	old_mouse_x = -1;
	projMode = false;

	config();
}

void VizMainWindow::config()
{
	flatMode = (bool) p->value ("flatMode", "visual", 0);
	int w = p->value ("width",  "visual", 640);
	int h = p->value ("height", "visual", 480);
	resize (w, h);
	p->rewrite();
}

/**
 * функция, которая вызывается через некоторые промежутки времени
 * int value - номер события
 */
void VizMainWindow::timer (int )
{
	processEvents();
}

void VizMainWindow::draw()
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	try
	{
		v_objs->draw();
	}
	catch (Exception &e)
	{
		cerr << e.toString() << endl;
	}
	glFlush();
	glutSwapBuffers();
}

void VizMainWindow::repaint()
{
//	glLoadIdentity();
//	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	draw();
}

/**
 * пока не используется
 */
void VizMainWindow::idle()
{
//	glutPostRedisplay();
}

void VizMainWindow::mouseMoveEvent (int x, int y)
{
	v_objs->mouseMoveEvent (x, y);
	int mouse_x = x, mouse_y = y;

	switch (last_mouse_button)
	{
	case 0:
		if (last_mouse_state == GLUT_DOWN)
		{
			xa += mouse_y - old_mouse_y;
			ya += mouse_x - old_mouse_x;
			rotate();

			old_mouse_x = mouse_x;
			old_mouse_y = mouse_y;
		}
		break;
	case 1:
		if (last_mouse_state == GLUT_DOWN)
		{
			y0 += mouse_y - old_mouse_y;
			x0 -= mouse_x - old_mouse_x;
			rotate();

			old_mouse_x = mouse_x;
			old_mouse_y = mouse_y;
		}
		break;
	default:
		break;
	}
}

void VizMainWindow::mousePressEvent (int button, int state, int x, int y)
{
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;
	GLdouble wx, wy, wz;

	last_mouse_button = button;
	last_mouse_state  = state;

	old_mouse_x = x;
	old_mouse_y = y;

	v_objs->mousePressEvent (button, state, x, y);
	switch (button)
	{
	case 0:
	case 1:
#if 0
		if (state == GLUT_DOWN)
		{
			old_mouse_x = x;
			old_mouse_y = y;

			//обратное преобразование для определения 3D координат мыши
			glGetIntegerv (GL_VIEWPORT, viewport);
			glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
			glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

			realy = viewport[3] - (GLint) y - 1;
			cout << "Cursor coordinates (" << x << "," << realy << ")" << endl;
			cout << "Cursor coordinates (" << x << "," << y << ")" << endl;
			//по оконным координатам x, y, z получаем мировые координаты
			//аналогичная команда gluProject по мировым координатам получает оконные
			// .,.,. <- изначальные координаты, матрицы преоброазований -> .,.,. получаемые координаты -
			//  - оконные или мировые, в зависимости от команды
			gluUnProject ( (GLdouble) x, (GLdouble) realy, 0.0, mvmatrix, projmatrix, viewport,
			               &wx, &wy, &wz);
			cout << "World coordinates at z = 0.0 (" << wx << "," << wy << "," << wz << ",)" << endl;
			gluUnProject ( (GLdouble) x, (GLdouble) realy, 1.0, mvmatrix, projmatrix, viewport,
			               &wx, &wy, &wz);
			cout << "World coordinates at z = 1.0 (" << wx << "," << wy << "," << wz << ",)" << endl;

			long t2 = clock();
			if ( (double) (t2 - last_clicked_time) / (double) CLOCKS_PER_SEC < 0.2)
			{
				v_objs->trySelect (x, realy);
				last_clicked_time = 0;
			}
			else
			{
				last_clicked_time = clock();
			}
		}
#endif
		break;
	case 3:
		if (flatMode || projMode)
		{
			zo *= 1.1;
			rotate();
			break;
		}
		else
		{
			zo -= 0.2;
		}
	case 4:
		if (flatMode || projMode)
		{
			zo /= 1.1;
		}
		else
		{
			zo += 0.1;
		}
		rotate();
		break;
	}
}

void VizMainWindow::keyPressEvent1 (unsigned char key, int x, int y)
{
	if (console->isHidden() )
	{
		v_objs->keyPressEvent1 (key, x, y);
		switch (key)
		{
		case '=':
		case '+':
			if (flatMode)
			{
				zo *= 1.1;
				rotate();
				break;
			}
			else
			{
				zo -= 0.2;
			}
		case '-':
			if (flatMode)
			{
				zo /= 1.1;
			}
			else
			{
				zo += 0.1;
			}
			rotate();
			break;
		case 'f':
			if (isFullscreen)
			{
				windowed();
			}
			else
			{
				fullscreen();
			}
			break;
		case 'p':
			if (projMode)
			{
				perspective();
			}
			else
			{
				orthogonal();
			}
			break;
		case 'q':
			v_objs->save();
			exit (0);
			break;
		default:
			break;
		}
	}
	else
	{
		console->keyPressEvent1 (key, x, y);
	}
}

void VizMainWindow::keyPressEvent2 (int key, int x, int y)
{
	if (console->isHidden() )
	{
		v_objs->keyPressEvent2 (key, x, y);

		if (!flatMode)
		{
			switch (key)
			{
			case GLUT_KEY_UP:
				xa += 5;// ya = 0; za = 0;
				break;
			case GLUT_KEY_DOWN:
				xa -= 5;// ya = 0; za = 0;
				break;
			case GLUT_KEY_RIGHT:
				ya += 5;// xa = 0; za = 0;
				break;
			case GLUT_KEY_LEFT:
				ya -= 5;// xa = 0; za = 0;
				break;
			}
		}
		else
		{
			switch (key)
			{
			case GLUT_KEY_UP:
				y0 -= 5;
				break;
			case GLUT_KEY_DOWN:
				y0 += 5;
				break;
			case GLUT_KEY_RIGHT:
				x0 -= 5;
				break;
			case GLUT_KEY_LEFT:
				x0 += 5;
				break;
			}
		}
		rotate();
	}
	else
	{
		console->keyPressEvent2 (key, x, y);
	}
}

void VizMainWindow::fullscreen()
{
	old_width = frame->width();
	old_height = frame->height();
	glutFullScreen();
	glutSetMenu (fileMenuDescriptor);
	glutChangeToMenuEntry (2, "Window Mode", 3);
	isFullscreen = true;
}

void VizMainWindow::windowed()
{
	glutReshapeWindow (old_width, old_height);
	glutPositionWindow (100, 100);
	glutSetMenu (fileMenuDescriptor);
	glutChangeToMenuEntry (2, "Fullscreen", 2);
	isFullscreen = false;
}

void VizMainWindow::perspective()
{
	projMode = false;
	resize(frame->width(), frame->height());
	reset_view();
	glutPostRedisplay();
}

void VizMainWindow::orthogonal()
{
	projMode = true;
	resize(frame->width(), frame->height());
	reset_view();
	glutPostRedisplay();
}

void VizMainWindow::fileMenu (int i)
{
	switch (i)
	{
	case 0:
		v_objs->save();
		exit (0);
		break;
	case 1:
		reset_view();
		break;
	case 2:
		fullscreen();
		break;
	case 3:
		windowed();
		break;
	case 4:
		if (console->isHidden() )
		{
			console->show();
		}
		else
		{
			console->hide();
		}
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void VizMainWindow::actionsMenu (int i)
{
}

void VizMainWindow::optionsMenu (int i)
{
}

void VizMainWindow::reset_view()
{
	xa = 0.0;
	ya = 0.0;
	za = 0.0;
	x0 = 0.0;
	y0 = 0.0;
	z0 = 0.0;

	if (flatMode || projMode)
	{
		zo = 1.0;
	}
	else
	{
		zo = 3.0;
	}

	rotate();
	glutPostRedisplay();
}

void VizMainWindow::resize (int width, int height)
{
	frame->resize (width, height);

	glViewport ( 0, 0, width, height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	if (!flatMode && !projMode)
	{
		gluPerspective (60.0, (GLdouble) width / (GLdouble) height, 0.1, 1000.0);
	}
	else
	{
		double k1 = (double) width  / (double) height;
		double k2 = 1;
		double r  = 1.5;
		glOrtho (r * -k1, r * k1, r * -k2, r * k2, -5, 5);
	}

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	/*    коорд камеры,  точка куда смотрим,  верх*/

	if (!flatMode && !projMode)
	{
		gluLookAt (0.0, 0.0, zo, x0, y0, z0, 0.0, 1.0, 0.0);
	}
	else
	{
		glScalef (zo, zo, 1.0);
		gluLookAt (x0, y0, 3, x0, y0, z0, 0.0, 1.0, 0.0);
	}
}

void VizMainWindow::registerEvents()
{
	glutDisplayFunc (global_repaint);
//	glutIdleFunc(global_idle);
	glutReshapeFunc (global_resize);
	glutMouseFunc (global_mousePressEvent);
	glutMotionFunc (global_mouseMoveEvent);
	glutKeyboardFunc (global_keyPressEvent1);
	glutSpecialFunc (global_keyPressEvent2);
	glutTimerFunc (100 /*m sec*/, global_timer, 0);
}

void VizMainWindow::createMenues()
{
	/* File Menu */
	fileMenuDescriptor = glutCreateMenu (global_fileMenu);
	glutSetMenu (fileMenuDescriptor);
//	glutAddMenuEntry("Open", 1);
//	glutAddMenuEntry("Close", 2);
//	glutAddMenuEntry("Save", 3);
	glutAddMenuEntry ("Reset View", 1);
	glutAddMenuEntry ("Fullscreen", 2);
	//на 3 "Window Mode !" не занимать !
	glutAddMenuEntry ("Console", 4);

	glutAddMenuEntry ("Quit", 0);
	glutAttachMenu (GLUT_RIGHT_BUTTON);

	/* Options Menu */
	//optionsMenuDescriptor = glutCreateMenu(global_optionsMenu);
	//glutSetMenu(optionsMenuDescriptor);
	//glutAddMenuEntry("Pallete", 0);
	//glutAddMenuEntry("File Format", 0);

	/* Actions Menu */
	//actionsMenuDescriptor = glutCreateMenu(global_actionsMenu);
	//glutSetMenu(actionsMenuDescriptor);
	//glutAddSubMenu("Options", optionsMenuDescriptor);
	//glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

void VizMainWindow::clear()
{
	glClearColor (0.8, 0.8, 0.8, 0.0); //закраска черным цветом
	glClearDepth (0.0); //сброс буфера глубины в нуль
	//GL_COLOR_BUFFER_BIT - очистить пиксели
	//GL_DEPTH_BUFFER_BIT - очистка буфера глубины
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
}

void VizMainWindow::initGL (int argc, char **argv)
{
	glutInit (&argc, argv);
	//двойная буферизация, rgb палитра, буфер глубины (для обрубания невидимых линий)
	//альфа канал (прозрачность?)
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ALPHA);
	glutInitWindowSize (frame->width(), frame->height() );
	glutCreateWindow (name.c_str() );
	glClearColor (0.8, 0.8, 0.8, 0.0);
	//glShadeModel(GL_FLAT);
	glEnable (GL_DEPTH_TEST);//для обрубания невидимых линий
	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);

//    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glLineWidth (1);

//	glLoadIdentity();
//	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

int VizMainWindow::exec (int argc, char **argv)
{
	xa = 0.0;
	ya = 0.0;
	za = 0.0;
	x0 = 0.0;
	y0 = 0.0;
	z0 = 0.0;

	if (flatMode || projMode)
	{
		zo = 1.0;
	}
	else
	{
		zo = 3.0;
	}

	isFullscreen = false;
	v_objs   = new Viz_List();
	v_events = new Viz_Event_Queue();
	mainwindow = this;
	initGL (argc, argv);
	init_basic_objects();
	registerEvents();
	createMenues();
	draw();
	glutMainLoop();
	return 1;
}

void VizMainWindow::init_basic_objects()
{
	v_objs->push_back ( (console = new Viz_Console (this) ) );

	if (p->value ("cube", "visual", 1) )
	{
		v_objs->push_back ( (new Viz_Cube (2.0) ) );
	}

//    v_objs->push_back((new Viz_Surface("PROGINI.DAT", Viz_Surface::sPlain)));
//    v_objs->push_back((new Viz_Surface("PROGINI.DAT", Viz_Surface::sNurbs)));
}

void VizMainWindow::save()
{
	v_objs->save();
}

void VizMainWindow::processEvents()
{
	try
	{
		v_events->process();
	}
	catch (Exception &e)
	{
		string name = e.toString();
		console->pushErrorString (name);
		v_events->clear_bad_events();
	}
	glutPostRedisplay();
}

void VizMainWindow::pushEvent (Viz_Event * e)
{
//!\todo warning thread unsafe!
	v_events->push (e);
}

//вернуть разрешение окна
Rect * VizMainWindow::getBoundingRect()
{
	return frame;
}

void VizMainWindow::rotate()
{
	double w = frame->width();
	double h = frame->height();
	glLoadIdentity();
	//cout << x0 << ":" << y0 << ":" << z0 << endl;

	if (!flatMode && !projMode)
	{
		gluLookAt (0.0, 0.0, zo, x0 / w, y0 / h, z0 / 2.0, 0.0, 1.0, 0.0);
		glRotatef (xa, 1.0, 0.0, 0.0);
		glRotatef (ya, 0.0, 1.0, 0.0);
	}
	else if (projMode && !flatMode)
	{
		glScalef (zo, zo, 1.0);
		gluLookAt (0.0, 0.0, zo, x0 / w, y0 / h, z0 / 2.0, 0.0, 1.0, 0.0);
		glRotatef (xa, 1.0, 0.0, 0.0);
		glRotatef (ya, 0.0, 1.0, 0.0);
	}
	else
	{
		glScalef (zo, zo, 1.0);
		gluLookAt (
		    x0 / w, y0 / h, 3,
		    x0 / w, y0 / h, z0,
		    0.0, 1.0, 0.0);
	}

	draw();
}

void VizMainWindow::clear_objs (int mask)
{
	v_objs->clear (mask);
}

void VizMainWindow::pushObj (Viz_Obj *o)
{
	v_objs->push_back (o);
	glutPostRedisplay();
}

Viz_List * VizMainWindow::objects()
{
	return v_objs;
}

