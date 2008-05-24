/*$Id: viz_obj.cpp 1619 2007-02-18 14:02:16Z manwe $*/

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

#ifdef _WIN32
#define GLUT_BUILDING_LIB
#endif
#include <GL/glut.h>
#include "viz_obj.h"

using namespace std;

Box::Box(int w1, int h1, int l1) {
	w = w1;
	h = h1;
	l = l1;
}

int Box::width() {
	return w;
}

int Box::height() {
	return h;
}

int Box::length() {
	return l;
};

void Box::resize(int w1, int h1, int l1) {
	w = w1; h = h1; l = l1;
}

Rect::Rect(int w1, int h1) {
	w = w1;
	h = h1;
    x0 = 0;
    y0 = 0;
}

int Rect::x() {
    return x0;
}

int Rect::y() {
    return y0;
}

bool Rect::isInside(int x, int y) {
    if (x0 < x && x < x0 + w && y0 < y && y < y0 + h) {
        return true;
    } else {
        return false;
    }
}

Rect::Rect(int x01, int y01, int w1, int h1) {
	w = w1;
	h = h1;
    x0 = x01;
    y0 = y01;
}

void Rect::move(int x01, int y01) {
	x0 = x01; y0 = y01;
}

void Rect::resize(int w1, int h1) {
	w = w1; h = h1;
}

int Rect::width() {
	return w;
}

int Rect::height() {
	return h;
}

Viz_Obj::Viz_Obj() {
    frame_ = 0;
	oType_ = oGeneral;
}

Viz_Obj::~Viz_Obj() {
	delete frame_;
}

//возвращает обрамляющий параллелепипед
Box  * Viz_Obj::getBoundingBox()
{
	return 0;
}

//возвращает область обзора
Rect * Viz_Obj::getBoundingRect() {
	return frame_;
}

void Viz_Obj::setDrawMode(DrawMode mode) {
    dMode_ = mode;
}

void Viz_Obj::rotate(double xa, double ya, double za) {
}

void Viz_Obj::mouseMoveEvent ( int x, int y ) {
}

void Viz_Obj::mousePressEvent ( int button, int state, int x, int y ) {
}

void Viz_Obj::keyPressEvent1 ( unsigned char key, int x, int y ) {
}

void Viz_Obj::keyPressEvent2 ( int key, int x, int y ) {
}

void Viz_Obj::show() {
}

bool Viz_Obj::isHidden() {
	return hidden_;
}

void Viz_Obj::hide() {
	if (hidden_)
		hidden_ = false;
	else
		hidden_ = true;
}

void Viz_Obj::save() {
}

bool Viz_Obj::trySelect(int, int) {
    return false;
}

bool Viz_Obj::selected() {
    return false;
}

int Viz_Obj::getType() {
	return oType_;
}

Viz_List::Viz_List() {
	auto_delete = true;
}

Viz_List::~Viz_List() {
	if (auto_delete) {
		for (int i = 0; i < (int)size(); i++) {
			delete at(i);
		}
	}
}

void Viz_List::setAutoDelete(bool t) {
	auto_delete = t;
}

void Viz_List::clear() {
	if (auto_delete) {
		for (int i = 0; i < (int)size(); i++) {
			delete at(i);
		}
	}
	vector < Viz_Obj *>::clear();
}

void Viz_List::clear(int mask) {
	vector <Viz_Obj * >::iterator Iter;
	bool cleared = false;
	while (!cleared) {
		cleared = true;
		for ( Iter = begin( ) ; Iter != end( ) ; Iter++ ) {
			if ((*Iter)->getType() & mask) {
				Viz_Obj * v = *Iter;
				vector < Viz_Obj * >::erase(Iter);
				delete v;
				cleared = false;
				break;
			}
		}
	}
}

bool Viz_List::trySelect(int x, int y) {
    bool ret = false;
	for (int i = 0; i < (int)size(); i++) {
		ret = ret || at(i)->trySelect(x, y);
    }
    return ret;
}

void Viz_List::erase(Viz_Obj * o) {
	for (vector < Viz_Obj * >::iterator it = begin(); it != end(); it++)
	{
		if (*it == o) {
			Viz_Obj * o = *it;
			vector < Viz_Obj * >::erase(it);
			if (auto_delete) {
				delete o;
			}
			return;
		}
	}
}

Viz_Obj * Viz_List::select(int x, int y) {
	for (int i = 0; i < (int)size(); i++) {
        if (at(i)->trySelect(x, y)) {
            return at(i);
        }
    }
    return 0;
}

void Viz_Obj::select() {
}

void Viz_List::draw() {
	for (int i = 0; i < (int)size(); i++) {
		at(i)->draw();
	}
}

void Viz_List::show() {
	for (int i = 0; i < (int)size(); i++) {
		at(i)->show();;
	}
}

void Viz_List::hide() {
	for (int i = 0; i < (int)size(); i++) {
		at(i)->hide();;
	}
}

void Viz_List::save() {
	for (int i = 0; i < (int)size(); i++) {
		at(i)->save();;
	}
}

void Viz_List::rotate(double xa, double ya, double za) {
	for (int i = 0; i < (int)size(); i++) {
		at(i)->rotate(xa, ya, za);;
	}
}

void Viz_List::mouseMoveEvent ( int x, int y ) {
	for (int i = 0; i < (int)size(); i++) {
		at(i)->mouseMoveEvent(x, y);;
	}
}

void Viz_List::mousePressEvent ( int button, int state, int x, int y ) {
	for (int i = 0; i < (int)size(); i++) {
		at(i)->mousePressEvent(button, state, x, y);;
	}
}

void Viz_List::keyPressEvent1 ( unsigned char key, int x, int y ) {
	for (int i = 0; i < (int)size(); i++) {
		at(i)->keyPressEvent1(key, x, y);;
	}
}

void Viz_List::keyPressEvent2 ( int key, int x, int y ) {
	for (int i = 0; i < (int)size(); i++) {
		at(i)->keyPressEvent2(key, x, y);;
	}
}

Viz_Cube::Viz_Cube(double size1) {
	oType_  = oSystem;
	size   = size1;
	hidden_ = false;
}

Viz_Cube::~Viz_Cube() {}

void Viz_Cube::draw() {
	if (hidden_)
		return;
	glColor3f(0.0, 0.0, 0.0);
	glutWireCube(size);
	glFlush();
}
