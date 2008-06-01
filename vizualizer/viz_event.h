#ifndef _VIZ_EVENT_H
#define _VIZ_EVENT_H
/*$Id: viz_event.h 1867 2008-05-23 21:11:01Z manwe $*/

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

#include <queue>
#include <string>
#include <vector>
#include "viz_obj.h"
#include "viz_console.h"
#include "asp_excs.h"
#include "tool_factory.h"

class Viz_Event {
protected:
	bool threadable; //обработчки вызывает отдельный тред?
	Viz_Obj * parent;
public:
	Viz_Event (Viz_Obj * parent = 0);
	virtual ~Viz_Event();
	virtual void process() = 0;
	virtual bool isThreadable();
};

typedef Viz_Event * (*CreateVizEventCallback)(Viz_Obj * parent, //!<объект к которому относится событие (MainWindow?)
											  Viz_Obj * sender, //!<отправитель события (Console?)
											  std::vector <std::string> & args/*!<аргументы*/);

template <
	class AbstractProduct,
	typename IdentifierType,
	typename ProductCreator,
	template <typename, class> class FactoryErrorPolicy = DefaultFactoryError
>
class Viz_Event_Factory: 
	public Factory < AbstractProduct, IdentifierType, ProductCreator >
{
	typedef typename Factory <AbstractProduct, IdentifierType, ProductCreator, FactoryErrorPolicy>::AssocMap AssocMap;
	using Factory <AbstractProduct, IdentifierType, ProductCreator, FactoryErrorPolicy>::assiciations_;
public:
	AbstractProduct * CreateObject(const IdentifierType & id,
		Viz_Obj * parent,
		Viz_Obj * sender,
		std::vector <std::string> & args)
	{
		typename AssocMap::const_iterator i 
			= assiciations_.find(id);
		if (i != assiciations_.end())
		{
			return (i->second)(parent, sender, args);
		}
		return FactoryErrorPolicy < IdentifierType, AbstractProduct >::OnUnknownType(id);
	}

	static Viz_Event_Factory & Instance()
	{
		static Viz_Event_Factory factory;
		return factory;
	}

private:
	Viz_Event_Factory() {}
	~Viz_Event_Factory() {}

	Viz_Event_Factory(const Viz_Event_Factory&);
	Viz_Event_Factory & operator = (const Viz_Event_Factory&);
};

typedef Viz_Event_Factory <Viz_Event, std::string, CreateVizEventCallback> TheViz_Event_Factory;

template < typename Event > 
Viz_Event * Viz_Event_Creator(Viz_Obj * parent, Viz_Obj * sender, 
							  std::vector <std::string> & args)
{
	return new Event(parent, sender, args);
}

class Viz_Descr {
public:
	typedef std::map < std::string, std::string > DescrMap;

	static Viz_Descr & Instance() {
		static Viz_Descr d;
		return d;
	}
	
	static bool Add_Descr(const std::string& key, const std::string& dscr)
	{
		Viz_Descr & d = Instance();
		d.Descr()[key] = dscr;
		return true;
	}

	DescrMap & Descr() {
		return descr;
	}

private:
	DescrMap descr;

	Viz_Descr()  {};
	~Viz_Descr() {};
	Viz_Descr(const Viz_Descr &);
	Viz_Descr&operator=(const Viz_Descr&);
};

#define REGISTER_EVENT(E, id, descr) \
	namespace { \
		static bool registred_##E = TheViz_Event_Factory::Instance().Register(id, Viz_Event_Creator<E>); \
		static bool dscr_##E      = Viz_Descr::Add_Descr(id, descr); \
	}

class Viz_Event_Queue: public std::queue < Viz_Event * >, Viz_Event {
	bool auto_delete;
	std::vector < Viz_Event * > unprocessed; //события, которые не могут быть обработыны
public:
	Viz_Event_Queue (Viz_Obj * parent = 0);
	virtual ~Viz_Event_Queue();
	virtual void setAutoDelete(bool t);
	virtual void process();
	virtual void clear_bad_events(); //очистка необрабатываемых событий
};

class Quit: public Viz_Event {
	int exit_code;
public:
	Quit(Viz_Obj * parent = 0, int code = 0);
	void process();
};

class ClearObjs: public Viz_Event {
	int mask;
public:
	ClearObjs(Viz_Obj * parent, int mask);
	void process();
};

class LoadMatrixFromFile: public Viz_Event {
protected:
	std::string  filename;

public:
	LoadMatrixFromFile(Viz_Obj * parent, const char * filename);
	virtual ~LoadMatrixFromFile() {}
	void process();
};

class LoadSphereFromFile: public LoadMatrixFromFile {
	int type;

public:
	LoadSphereFromFile(Viz_Obj * parent, const char * filename, const char * type = 0);
	virtual ~LoadSphereFromFile() {}
	void process();
};

class ListObjs: public Viz_Event {
	Viz_Console * console_;
public:
	ListObjs(Viz_Obj * m_win, Viz_Obj * console, std::vector < std::string > &args);
	virtual ~ListObjs();
	void process();
};

REGISTER_EVENT(ListObjs, "list", "lists objects");

//!выделяет объект с номером
class SelectObj: public Viz_Event {
protected:
	Viz_Console * console_;
	std::vector < std::string > args_;

public:
	SelectObj(Viz_Obj * m_win, Viz_Obj * console, std::vector < std::string > &args);
	virtual ~SelectObj();
	void process();
};

REGISTER_EVENT(SelectObj, "select", "select object with id");

//!удаляет выделенный объект или объект с нужным id
class DeleteObj: public SelectObj {
public:
	DeleteObj(Viz_Obj * m_win, Viz_Obj * console, std::vector < std::string > &args);
	void process();
};

REGISTER_EVENT(DeleteObj, "delete", "delete selected object or object with id");

//!скрывает выделенный объект или объект с нужным id
class HideObj: public SelectObj {
public:
	HideObj(Viz_Obj * m_win, Viz_Obj * console, std::vector < std::string > &args);
	void process();
};

REGISTER_EVENT(HideObj, "hide", "hide selected object or object with id");

//!сохраняет скриншот
class SaveScreen: public Viz_Event {
	std::string file_name_;
	Viz_Console * console_;

public:
	SaveScreen(Viz_Obj * win, Viz_Obj * console, 
		std::vector < std::string > &args);
	void process();
};

REGISTER_EVENT(SaveScreen, "screen", "save screenshot to ppm format");

class LoadTriang: public Viz_Event {
	std::string file_name_;
	Viz_Console * console_;

public:
	LoadTriang(Viz_Obj * win, Viz_Obj * console, 
		std::vector < std::string > &args);
	void process();
};

REGISTER_EVENT(LoadTriang, "triang", "loads trinagles from file");

class LoadSegmentsFromFile: public Viz_Event {
	std::string file_name_;
	Viz_Console * console_;

public:
	LoadSegmentsFromFile(Viz_Obj * win, Viz_Obj * console, 
		std::vector < std::string > &args);
	virtual ~LoadSegmentsFromFile() {}
	void process();
};

REGISTER_EVENT(LoadSegmentsFromFile, "segments", "loads segments from file");

//!выполняет построчно команды из файла
class Batch: public Viz_Event {
	std::string file_name_;
	Viz_Console * console_;

public:
	Batch(Viz_Obj * console, std::vector < std::string > &args);
	void process();
};

namespace {
	Viz_Event * CreateBatch(Viz_Obj * parent, Viz_Obj * sender, 
							  std::vector <std::string> & args)
	{
		return new Batch(sender, args);
	}

	bool batch_registred = TheViz_Event_Factory::Instance().Register("batch", CreateBatch);
	bool batch_descr     = Viz_Descr::Add_Descr("batch", "run commands from file");

	Viz_Event * CreateQuit(Viz_Obj * parent, Viz_Obj * sender, 
						   std::vector <std::string> & args)
	{
		int code = 0;
		if (!args.empty()) {
			code = atoi(args[0].c_str());
		}
		return new Quit(parent, code);
	}

	bool quit_registred = TheViz_Event_Factory::Instance().Register("quit", CreateQuit);
	bool quit_descr     = Viz_Descr::Add_Descr("quit", "exit");

	bool quit_registred2= TheViz_Event_Factory::Instance().Register("exit", CreateQuit);
	bool quit_descr2    = Viz_Descr::Add_Descr("exit", "exit");

	Viz_Event * CreateClear(Viz_Obj * parent, Viz_Obj * sender, 
						   std::vector <std::string> & args)
	{
		return new ClearObjs(parent, Viz_Obj::oGeneral);
	}

	bool clear_registred = TheViz_Event_Factory::Instance().Register("clear", CreateClear);
	bool clear_descr     = Viz_Descr::Add_Descr("clear", "clears all");

	Viz_Event * CreateMatrix(Viz_Obj * parent, Viz_Obj * sender, 
						   std::vector <std::string> & args)
	{
		if (args.empty())
			throw BadArgument("empty argument list");

		return new LoadMatrixFromFile(parent, args[0].c_str());
	}

	bool matrix_registred = TheViz_Event_Factory::Instance().Register("matrix", CreateMatrix);
	bool matrix_descr     = Viz_Descr::Add_Descr("matrix", "draw matrix from file");

	Viz_Event * CreateSphere(Viz_Obj * parent, Viz_Obj * sender, 
						   std::vector <std::string> & args)
	{
		return new LoadSphereFromFile(parent, args[0].c_str(), 
				(args.size() > 1) ? args[1].c_str() : 0);
	}

	bool sphere_registred = TheViz_Event_Factory::Instance().Register("sphere", CreateSphere);
	bool sphere_descr     = Viz_Descr::Add_Descr("sphere", "draw sphere from file");


}
#endif //_VIZ_EVENT_H
