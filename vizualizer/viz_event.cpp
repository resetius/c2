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

#ifdef _WIN32
#define GLUT_BUILDING_LIB
#endif
#include <GL/glut.h>

#include "viz_event.h"
#include "viz_mainwindow.h"
#include "viz_surface.h"
#include "viz_sphere.h"
#include "viz_segments.h"
#include "viz_triang.h"

using namespace std;

#ifdef _WIN32
#define snprintf _snprintf
#endif

Viz_Event::Viz_Event (Viz_Obj * p) 
{
	parent = p;
	threadable = false;
}

Viz_Event::~Viz_Event() {}

bool Viz_Event::isThreadable() 
{
	return threadable;
}

Viz_Event_Queue::Viz_Event_Queue (Viz_Obj * p): Viz_Event(p)
{
	auto_delete = true;
}

Viz_Event_Queue::~Viz_Event_Queue()
{
	if (auto_delete) {
		while (!empty()) {
			delete front();
			pop();
		}
	}
}

void Viz_Event_Queue::setAutoDelete(bool t)
{
	auto_delete = t;
}

void Viz_Event_Queue::clear_bad_events() {
	for (unsigned int i = 0; i < unprocessed.size(); i++) {
		Viz_Event * e = unprocessed[i];
		if (e != 0) {
			delete e;
		}
	}
	unprocessed.clear();
}

void Viz_Event_Queue::process()
{
	while (!empty()) {
//!\todo warning thread unsafe!		
		//если обработчик исполняется в отдельном треде, то будет плохо
		Viz_Event * e = front();
		pop();
		unprocessed.push_back(e);
		e->process();
		if (auto_delete) delete e;
		//unprocessed.back() = 0;
		unprocessed.erase(unprocessed.end() - 1);
	}
	clear_bad_events();
}

Quit::Quit(Viz_Obj * parent, int code): Viz_Event(parent) {
	exit_code = code;
}

void Quit::process() {
	if (parent != 0) {
		parent->save();
	}
	exit(exit_code);
}

ClearObjs::ClearObjs(Viz_Obj * parent, int mask1): Viz_Event(parent) {
	mask = mask1;
}

void ClearObjs::process() {
	((VizMainWindow*)parent)->clear_objs(mask);
}

/* _____________MATRIX_________________________________________*/

LoadMatrixFromFile::LoadMatrixFromFile(Viz_Obj * parent, 
									   const char * filename1) 
: Viz_Event(parent), filename(filename1)
{
}

void LoadMatrixFromFile::process() 
{
	Viz_Surface * s = new Viz_Surface(filename.c_str());
	dynamic_cast<VizMainWindow*>(parent)->pushObj(s);
}

/* ______________SPHERE________________________________________*/
LoadSphereFromFile::LoadSphereFromFile(Viz_Obj * parent, const char * filename, 
									   const char * type_)
	: LoadMatrixFromFile(parent, filename)
{
	//printf("%s\n", type_);
	if (type_ == 0) {
		type = Viz_Sphere::sHalf;
	} else {
		if (!strcmp(type_, "full")) {
			type = Viz_Sphere::sFull;
		} else if (!strcmp(type_, "flat")) {
			type = Viz_Sphere::sFlat;
		} else {
			type = Viz_Sphere::sHalf;
		}
	}
}

void LoadSphereFromFile::process() {
	Viz_Sphere * s = new Viz_Sphere(filename.c_str(), type);
	dynamic_cast<VizMainWindow*>(parent)->pushObj(s);
}

ListObjs::~ListObjs() {
}

ListObjs::ListObjs(Viz_Obj * m_win, Viz_Obj * console, std::vector < std::string > &)
	: Viz_Event(m_win) 
{
	console_ = dynamic_cast<Viz_Console*>(console);
	if (!console_)
		throw BadArgument("FATAL: invalid sender");
}

void ListObjs::process() {
	VizMainWindow * m_win = (VizMainWindow *)parent;
	const Viz_List * list = m_win->objects();
	char buf[32768];
    snprintf(buf, 32767, "Object Id  | Type Id | is selected ");
    console_->pushOutputString(buf);
	for (size_t i = 0; i < list->size(); i++) {
        Viz_Obj * o = list->at(i);
        if (o->selected()) {
            snprintf(buf, 32767, "%ld | %d | * ", (long)(o), o->getType());
        } else {
    		snprintf(buf, 32767, "%ld | %d |", (long)(o), o->getType());
        }
		console_->pushOutputString(buf);
	}
}

SelectObj::SelectObj(Viz_Obj * m_win, Viz_Obj * console, vector < string > &args)
	: Viz_Event(m_win) 
{
	console_ = dynamic_cast<Viz_Console*>(console);
	if (!console_)
		throw BadArgument("FATAL: invalid sender");
	args_    = args;
}

DeleteObj::DeleteObj(Viz_Obj * m_win, Viz_Obj * console, vector < string > &args)
	: SelectObj(m_win, console, args)
{}

HideObj::HideObj(Viz_Obj * m_win, Viz_Obj * console, vector < string > &args)
	: SelectObj(m_win, console, args)
{}

SelectObj::~SelectObj() {
}

void SelectObj::process() {
	if (args_.empty()) {
		throw BadArgument("empty argument list");
	}

	VizMainWindow * m_win = dynamic_cast<VizMainWindow *>(parent);
	if (!m_win) {
		throw BadArgument("FATAL: invalid parent", __FILE__, __LINE__);
	}

	const Viz_List * list = m_win->objects();
	long id;
	if (sscanf(args_[0].c_str(), "%ld", &id) <=0) {
#ifdef _DEBUG
		throw BadArgument("bad argument", __FILE__, __LINE__);
#else
		throw BadArgument("bad argument");
#endif
	}
	for (size_t i = 0; i < list->size(); i++) {
		if ((long)(list->at(i)) == id) {
			list->at(i)->select();
			return;
		}
	}
#ifdef _DEBUG
	throw NotFound("object ", __FILE__, __LINE__);
#else
	throw NotFound("object");
#endif
}

void DeleteObj::process() {
	VizMainWindow * m_win = dynamic_cast<VizMainWindow *>(parent);
	if (!m_win) {
		throw BadArgument("FATAL: invalid parent", __FILE__, __LINE__);
	}

	Viz_List * list = m_win->objects();
	long id = 0;
	if (!args_.empty())
		if (sscanf(args_[0].c_str(), "%ld", &id) <= 0) id = 0;
	if (id != 0) {
		for (unsigned int i = 0; i < list->size(); i++) {
			if ((long)(list->at(i)) == id) {
				Viz_Obj * o = list->at(i);
				if ((o->getType() & Viz_Obj::oSystem) == 0)
					list->erase(o);
				return;
			}
		}
#ifdef _DEBUG
		throw NotFound("object ", __FILE__, __LINE__);
#else
		throw NotFound("object");
#endif
	} else {
		Viz_List to; to.setAutoDelete(false);
		for (unsigned int i = 0; i < list->size(); i++) {
			if (list->at(i)->selected()) {
				to.push_back(list->at(i));
			}
		}
		for (unsigned int i = 0; i < to.size(); i++) {
			if ((to[i]->getType() & Viz_Obj::oSystem) == 0)
				list->erase(to[i]);
		}
	}
}

void HideObj::process() {
	VizMainWindow * m_win = dynamic_cast<VizMainWindow *>(parent);
	if (!m_win) {
		throw BadArgument("FATAL: invalid parent", __FILE__, __LINE__);
	}

	Viz_List * list = m_win->objects();
	long id = 0;
	if (!args_.empty())
		if (sscanf(args_[0].c_str(), "%ld", &id) <= 0) id = 0;
	if (id != 0) {
		for (size_t i = 0; i < list->size(); i++) {
			if ((long)(list->at(i)) == id) {
				Viz_Obj * o = list->at(i);
				o->hide();
				return;
			}
		}
#ifdef _DEBUG
		throw NotFound("object ", __FILE__, __LINE__);
#else
		throw NotFound("object");
#endif
	} else {
		for (size_t i = 0; i < list->size(); i++) {
			Viz_Obj * o = list->at(i);
			if (o->selected())
				o->hide();
		}
	}
}

/* ____________TRIANG ______________ */
LoadTriang::LoadTriang(Viz_Obj * win, Viz_Obj * console, vector < std::string > & args)
	: Viz_Event(win)
{
	console_ = dynamic_cast<Viz_Console*>(console);
	if (!console_)
		throw BadArgument("FATAL: invalid sender");
	if (args.empty()) {
		throw BadArgument("empty argument list");
	}
	file_name_ = args[0];
}

void LoadTriang::process()
{
	Viz_Triang * s = new Viz_Triang(file_name_.c_str());
	dynamic_cast<VizMainWindow*>(parent)->pushObj(s);
}

/* ________________SEGMENTS______________________________________*/
LoadSegmentsFromFile::LoadSegmentsFromFile(Viz_Obj * win, 
			Viz_Obj * console, vector < std::string > & args)
	: Viz_Event(win)
{
	console_ = dynamic_cast<Viz_Console*>(console);
	if (!console_)
		throw BadArgument("FATAL: invalid sender");
	if (args.empty()) {
		throw BadArgument("empty argument list");
	}
	file_name_ = args[0];
}

void LoadSegmentsFromFile::process()
{
	dynamic_cast<VizMainWindow*>(parent)->pushObj(new Viz_Segments(file_name_.c_str()));
}

/* ______________ SAVE SCREEN __________________________*/
SaveScreen::SaveScreen(Viz_Obj * win, Viz_Obj * console, vector < std::string > & args)
	: Viz_Event(win)
{
	console_ = dynamic_cast<Viz_Console*>(console);
	if (!console_)
		throw BadArgument("FATAL: invalid sender");
	if (args.empty()) {
		throw BadArgument("empty argument list");
	}
	file_name_ = args[0];
}

void SaveScreen::process()
{
	VizMainWindow * m_win = 
		dynamic_cast<VizMainWindow*>(parent);
	console_->hide();
	if (m_win)
		m_win->draw();
	glutPostRedisplay();
	m_win->save_to_postscript(file_name_.c_str());
}

Batch::Batch(Viz_Obj * console, std::vector < std::string > &args)	
{
	console_ = dynamic_cast<Viz_Console*>(console);
	if (!console_)
		throw BadArgument("FATAL: invalid sender");

	if (args.empty()) {
		throw BadArgument("empty argument list");
	}
	file_name_ = args[0];
}

void Batch::process()
{
	FILE * f = fopen(file_name_.c_str(), "r");
	if (!f) return;
	vector < string > strs;
#define buf_size 32768
	char buf [buf_size];

	while (fgets(buf, buf_size - 1, f) != 0) {
		int l = strlen(buf);
		for (int i = l; i >= 0; --i) {
			if (buf[i] == '\n' || buf[i] == '\r') {
				buf[i] = 0;
			}
		}
		string s(buf);
		strs.push_back(s);
	}

	vector < string >::iterator it, st = strs.begin(), en = strs.end();
	for (it = st; it != en; ++it) {
		console_->processCommandString(*it);
	}
	fclose(f);
}
