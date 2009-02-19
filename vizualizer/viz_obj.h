#ifndef _VIZ_OBJ_H
#define _VIZ_OBJ_H
/*$Id$*/

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

#include <vector>
#include <math.h>

class Box
{
	int w;
	int h;
	int l;

public:
	Box (int w, int h, int l);
	virtual ~Box() {}
	virtual int width();
	virtual int height();
	virtual int length();
	virtual void resize (int w, int h, int l);
};

class Rect
{
	int x0;
	int y0;
	int w;
	int h;

public:
	Rect (int w, int h);
	Rect (int x0, int y0, int w, int h);
	virtual ~Rect() {}
	virtual int width();
	virtual int height();
	virtual int x();
	virtual int y();
	virtual bool isInside (int x, int y);
	virtual void resize (int w, int h);
	virtual void move (int x0, int y0);
};

class Viz_Obj
{

public:
	//режим отображения
	enum DrawMode
	{
		dPoints = 0, //точками
		dLines  = 1, //линиями (wireframe)
		dFill   = 2,  //закрашенные многоугольники
		dEnd    = 3,
	};

	enum ObjType
	{
		oSystem  = 1, //системный объект
		oGeneral = 2,
		oSurface = 4
	};

	virtual void setDrawMode (DrawMode mode);
	virtual int getType();

	Viz_Obj();
	virtual ~Viz_Obj();

	virtual void draw() = 0;
	virtual void show();
	virtual void hide();
	virtual void save();
	virtual bool isHidden();
	virtual bool trySelect (int x, int y);
	virtual void select();
	virtual bool selected();

	virtual void rotate (double xa, double ya, double za);
	virtual void mouseMoveEvent ( int x, int y );
	virtual void mousePressEvent ( int button, int state, int x, int y );
	virtual void keyPressEvent1 ( unsigned char key, int x, int y );
	virtual void keyPressEvent2 ( int key, int x, int y );

	virtual Box  * getBoundingBox();  //возвращает обрамляющий параллелепипед
	virtual Rect * getBoundingRect(); //возвращает область обзора

protected:
	int  oType_;
	bool hidden_;
	Rect * frame_;
	int dMode_;
};

class Viz_Point
{
public:
	// global 3D coordinates
	double d[3];

	double &x;
	double &y;
	double &z;

	// function
	double f;

	// local 2D coordinates (for surface)
	double u;
	double v;

	Viz_Point (double x1, double y1, double z1, 
		double f1 = 0.0, double u1 = 0.0, double v1 = 0.0) 
		:  x(d[0]), y(d[1]), z(d[2]), f(f1), u(u1), v(v1) 
	{
		d[0] = x1;
		d[1] = y1;
		d[2] = z1;
	}

	Viz_Point() : x(d[0]), y(d[1]), z(d[2]), f(0), u(0), v(0) 
	{
		d[0] = 0;
		d[1] = 0;
		d[2] = 0;
	}

	Viz_Point(const Viz_Point & p): x(d[0]), y(d[1]), z(d[2])
	{
		d[0] = p.x;
		d[1] = p.y;
		d[2] = p.z;

		f = p.f;
		u = p.u;
		v = p.v;
	}

	Viz_Point & operator = (const Viz_Point & p)
	{
		d[0] = p.x;
		d[1] = p.y;
		d[2] = p.z;

		f = p.f;
		u = p.u;
		v = p.v;

		return *this;
	}
};

inline double dist(const Viz_Point & p1, const Viz_Point & p2)
{
	return sqrt((p1.x - p2.x) * (p1.x - p2.x) 
		+ (p1.y - p2.y) * (p1.y - p2.y) 
		+ (p1.z - p2.z) * (p1.z - p2.z));
}

class Viz_List: public std::vector < Viz_Obj * >, Viz_Obj
{
	bool auto_delete;

public:
	Viz_List();
	virtual ~Viz_List();
	void setAutoDelete (bool t);
	void clear();
	void clear (int mask);
	void draw();
	void show();
	void hide();
	void save();
	void erase (Viz_Obj * o);
	bool trySelect (int x, int y);
	Viz_Obj * select (int x, int y);
	void rotate (double xa, double ya, double za);
	void mouseMoveEvent ( int x, int y );
	void mousePressEvent ( int button, int state, int x, int y );
	void keyPressEvent1 ( unsigned char key, int x, int y );
	void keyPressEvent2 ( int key, int x, int y );
};

class Viz_Cube: public Viz_Obj
{
	double size;
public:
	Viz_Cube (double size);
	virtual ~Viz_Cube();
	void draw();
};
#endif //_VIZ_OBJ_H
