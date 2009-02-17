#ifndef _VIZ_CONSOLE_H
#define _VIZ_CONSOLE_H
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
#include <string>
#include <map>
#include "viz_obj.h"

class BufferItem;
class ConsoleBuffer: public std::vector < BufferItem > {
public:	
	enum StringType {
		sInput  = 0, //!<строка ввода
		sError  = 1, //!<ошибка
		sOutput = 2  //!<ответ системы
	};

	ConsoleBuffer();
	~ConsoleBuffer();

	std::string str(int i); //!<i'я строка
	std::string pre(int i); //!<префикс i'й строки
	float* color(int i); //!<цвета i'й строки
	float* colorOfType(int );//!<получить цвет по типу строки

	int type(int i); //!<тип i'й строки

	void push_back(BufferItem &b);
	void push_back(const std::string &s, int t = sInput); 
	//void removeFirst(); //удаляет первый элемент
	std::string &backStr();
	void backAppend(int num, char c);

private:
	std::map < int, float*   > color_map;   //!<цветовая карта
	std::map < int, std::string   > off_symb_map;//!<символы отступа
};

class BufferItem {
public:
	~BufferItem();
	BufferItem(const std::string &s1, int t = ConsoleBuffer::sInput);

	std::string & str();              //!<строка
	int type(); //!<вернуть тип строки
private:
	std::string     s;
	int stype; //!<тип строки
};

class Viz_Console: public Viz_Obj {
	int offset;  //номер строки в буфере с которой начинаем печать (для прокрутки)
	Viz_Obj * win;
	ConsoleBuffer     buffer;
	const static int buffer_max_size = 1000;
	int last; //id команды (для перебора команд стрелками)

public:
	Viz_Console(Viz_Obj * parent);
	virtual ~Viz_Console();
	
	void draw();
	void show();
	void hide();
	void keyPressEvent1 ( unsigned char key, int x, int y );
	void keyPressEvent2 ( int key, int x, int y );	
	void pushInputString (const std::string &s);
	void pushErrorString (const std::string &s);
	void pushOutputString (const std::string &s);
	void pushString (const std::string &s, int type = ConsoleBuffer::sOutput);
	void printCommandsList();
	void printHelp();
	void save();

	void processCommandString(const std::string &command_string);
protected:
	/**
	 * печать буфера начиная с координат x, y.
	 * печать идет снизу вверх
	 * размеры окна печати w*h
	 */
	void printBuffer(float x, float y, float w, float h); 
	void printStr(const std::string &s); //вывод строки по текущем координатам
	void processCommand(const std::string & command, std::vector <std::string> &args);
	void saveHistory();
	void loadHistory();

private:
	void dirListing();
	void changeDir(std::vector <std::string> &args);
};
#endif //_VIZ_CONSOLE_H
