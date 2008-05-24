#ifndef _VIZ_MAINWINDOW_H
#define _VIZ_MAINWINDOW_H
/*$Id: viz_mainwindow.h 622 2005-12-02 17:57:57Z manwe $*/

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

#include <string>
#include "asp_excs.h"
#include "asp_properties.h"
#include "viz_obj.h"
#include "viz_event.h"
#include "viz_console.h"

class VizMainWindow;

class VizMainWindow: public Viz_Obj {
	Properties *p;
	std::string name;
	Rect * frame; //разрешение width*height
	Viz_List        * v_objs;
	Viz_Console     * console;
	Viz_Event_Queue * v_events;
	
	double xa, ya, za; //углы обзора
	double x0, y0, z0; //точка в которую смотрим
	double zo;         //зум

	int old_mouse_x, old_mouse_y; //старые координаты мыши в момент нажатия
	int old_width, old_height;    //для перехода из fullscreen обратно

	int last_mouse_button;
	int last_mouse_state;

    long last_clicked_time;

public:
	~VizMainWindow();
	VizMainWindow();
	VizMainWindow(char *name);
	VizMainWindow(char *name, int w, int h);

	int exec(int argc, char **argv);
	void draw();
	void repaint();
	void idle();
	void resize(int width, int height);
	void rotate();
	
	void timer(int value); //обработка периодических событий. value - идентификатор события
	void mouseMoveEvent ( int x, int y );
	void mousePressEvent ( int button, int state, int x, int y );
	void keyPressEvent1 ( unsigned char key, int x, int y );
	void keyPressEvent2 ( int key, int x, int y );
	
	void fileMenu(int i);
	void actionsMenu(int i);
	void optionsMenu(int i);
	
	void pushEvent(Viz_Event *);
	void pushObj(Viz_Obj *);
	void processEvents();
	void clear_objs(int mask);
	void save();
	Viz_List * objects();
	
	Rect * getBoundingRect();//вернуть разрешение окна
private:
	bool isFullscreen;
	void createMenues();
	void registerEvents();
	void initGL(int argc, char **argv);
	void init_basic_objects();
	void clear();
	void reset_view();
	void fullscreen();
	void windowed();
	
	int fileMenuDescriptor;
	int actionsMenuDescriptor;
	int optionsMenuDescriptor;
};

#endif //_VIZ_MAINWINDOW_H
