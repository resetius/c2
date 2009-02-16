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
#include <direct.h>
#endif
#include <GL/glut.h>
#include <iostream>
#include "viz_console.h"
#include "viz_mainwindow.h"

#include "asp_readdir.h"

using namespace std;

ConsoleBuffer::~ConsoleBuffer() {
}

ConsoleBuffer::ConsoleBuffer() {
	//цвета трех типов вывода
	float t_cInput[]  = {0.9, 0.9, 0.9, 0.5};
	float t_cOutput[] = {0.0, 1.0, 1.0, 0.5};
	float t_cError[]  = {1.0, 0.8, 0.0, 0.5};
	float *cInput  = new float[4];
	float *cOutput = new float[4];
	float *cError  = new float[4];

	memcpy(cInput,  t_cInput,  4 * sizeof(float));
	memcpy(cOutput, t_cOutput, 4 * sizeof(float));
	memcpy(cError,  t_cError,  4 * sizeof(float));

	string preInput  = ">";
	string preOutput = " ";
	string preError  = "!";

	color_map[sInput]  = cInput;
	color_map[sOutput] = cOutput;
	color_map[sError]  = cError;

	off_symb_map[sInput]  = preInput;
	off_symb_map[sOutput] = preOutput;
	off_symb_map[sError]  = preError;
}

Viz_Console::Viz_Console(Viz_Obj * parent) {
	win    = parent;
	hidden_= true;
	oType_ = oSystem;
	offset = 0;
	last   = 0;
    pushInputString("");
	loadHistory();
}

Viz_Console::~Viz_Console() {
}
	
void Viz_Console::draw() {
	Rect * frame = win->getBoundingRect();
	if (frame == 0) return;
	int w = frame->width();
	int h = frame->height();

	if (!hidden_) {
		//glDisable(GL_DEPTH_TEST);       // Disables Depth Testing
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMatrixMode(GL_PROJECTION);    // Select The Projection Matrix
		glPushMatrix();                 // Store The Projection Matr
		glLoadIdentity();               // Reset The Projection Matr
		glOrtho(0, w, 0, h, -2, 2);     // Set Up An Ortho Screen
		glMatrixMode(GL_MODELVIEW);     // Select The Modelview Matrix
		glPushMatrix();                 // Store The Modelview Matrix
		glLoadIdentity();               // Reset The Modelview Matrix
		glTranslatef(0, 0, 1);          // Position The Text (0,0 - Bottom L

		//glDepthMask(GL_FALSE);
		//рисуем консоль
		glColor4f(0.5, 0.5, 0.5, 0.6);
		glRectf(0.0, 1.0 / 2.0 * h, w, h);
		
		glTranslatef(0, 0, 1);
		printBuffer(2, 1.0 / 2.0 * h + 2, w - 4, h - 1.0 / 2.0 * h);

		//glDepthMask(GL_TRUE);

		glMatrixMode(GL_PROJECTION);  // Select The Projection Matrix
		glPopMatrix();                // Restore The Old Projection
		glMatrixMode(GL_MODELVIEW);   // Select The Modelview Matrix
		glPopMatrix();                // Restore The Old Projectio
		//glEnable(GL_DEPTH_TEST);      // Enables Depth Testing
		//glDisable(GL_BLEND);
	}
}

void Viz_Console::printBuffer(float x, float y, float w, float h) {
	if (offset < 0) offset = 0;
	if (offset > (int)buffer.size() - 1) offset = buffer.size() - 1;
	if (offset == 0) {
		if (buffer.size() > 0) {
			int i = buffer.size() - 1;
			float * color = buffer.color(i);
			string out = buffer.pre(i) + buffer.str(i);
			glColor4fv(color);
			glRasterPos2f(x, y);
			printStr(out);
		}

		if (buffer.size() > 1) {
			for (int i = buffer.size() - 2, j = 1; i >= 0; i--, j++) {
				float * color = buffer.color(i);
				string out = buffer.pre(i) + buffer.str(i);
				glColor4fv(color);
				glRasterPos2f(x, y + 14 * j);
				printStr(out);
			}
		}
	} else if (buffer.size() > 1) {
		float * color = buffer.colorOfType(ConsoleBuffer::sOutput);
		glColor4fv(color);
		glRasterPos2f(x, y);
		for (int i = 0; i < w / 13; i++) {
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '^');
		}
		for (int i = buffer.size() - offset - 1, j = 1; i >= 0; i--, j++) {
			string out = buffer.pre(i) + buffer.str(i);
			glColor4fv(buffer.color(i));
			glRasterPos2f(x, y + 14 * j);
			printStr(out);
		}
	}
}

void Viz_Console::printStr(string &s) {
	const char * cs = s.c_str();
    while ( *cs != 0 ) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *cs++);
	}
}

void Viz_Console::printStr(const char * cs) {
    while ( *cs != 0 ) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *cs++);
	}
}

void Viz_Console::show() {
	offset = 0;
	last   = 0;
	hidden_= false;
}

void Viz_Console::hide() {
	offset = 0;
	last   = 0;
	hidden_= true;
}

void Viz_Console::processCommandString(string &command_string) {
	string command;
	vector < string > args;
	int c_old = 0;
	int len   = command_string.length();
	int c     = command_string.find(' ');
	if (c <= 0) {
		command = command_string;
	} else {
		command = string(command_string, 0, c);
		while (c_old != c && c < len - 1) {
			c_old = c;
			c = command_string.find(' ', c_old + 1);
			if (c <= 0) c = len - 1;
			string arg(command_string, c_old + 1, c - c_old);
			args.push_back(arg);
		}
	}

	processCommand(command, args);
}

void Viz_Console::processCommand(string & command, vector <string> &args) {
    VizMainWindow * m_win = dynamic_cast<VizMainWindow*>(win);
	try {
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*_____________________ internal commands _____________________________________*/
	if (command == "help") {
		printHelp();
	} else if (command == "ls") {
		dirListing();
	} else if (command == "pwd") {
		string str = getCurDir();
		pushString(str);
	} else if (command == "cd") {
		changeDir(args);
	} 
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*_____________________ external commands _____________________________________*/
	//по хорошему это всё надо переделать в фабрику объектов
	else {
		m_win->pushEvent(TheViz_Event_Factory::Instance().CreateObject(command, win, this, args));
	}
	} catch (exception &e) {
		pushErrorString(e.what());
	}
	glutPostRedisplay();
}

void Viz_Console::changeDir(vector <string> &args)
{
	if (args.empty()) return;
#ifdef _WIN32
	_chdir(args[0].c_str());
#else
	chdir(args[0].c_str());
#endif
}

void Viz_Console::dirListing()
{
	vector < string > dirs;
	string folder = ".";
	readFolder(dirs, folder);
	for (vector < string >::iterator it = dirs.begin(); it != dirs.end(); it++)
	{
		pushString(*it);
	}
}

void Viz_Console::keyPressEvent1 ( unsigned char key, int x, int y ) {
	offset = 0;
	switch (key) {
	case '~':
	case '`':
		if (isHidden()) {
			show();
		} else {
			hide();
		}
		glutPostRedisplay();
		break;
	case 9: //tab
		if (!isHidden()) {
			printCommandsList();
			glutPostRedisplay();
		}
		break;
	case 8: //backspace
		if (!isHidden()) {
			if (!buffer.empty()) {
				string &s = buffer.back().str();
				if (s.length() > 0) {
					s.erase(s.length() - 1);
				}
				glutPostRedisplay();
			}
		}
		break;
	case 13: //enter
		if (!isHidden()) {
			last = 0;
			string command_str = buffer.back().str();
			buffer.push_back("");

			//тут вставить обработку buffer.back()
			processCommandString(command_str);

			glutPostRedisplay();
		}
		break;
	default:
		if (!isHidden()) {
			if (isprint(key)) {
				if (buffer.size() == 0) {
					string s(string(1, key));
					buffer.push_back(s);
				} else {
                    buffer.backAppend(1, key);
				}
				glutPostRedisplay();
			}
		}
		break;
	}
}

void Viz_Console::keyPressEvent2 ( int key, int x, int y ) {
	if (!isHidden()) {
		int mod = glutGetModifiers();
		switch (mod) {
		case GLUT_ACTIVE_SHIFT:
			switch (key) {
			case GLUT_KEY_UP:
				offset += 2;
			case GLUT_KEY_DOWN:
				offset -= 1;
				glutPostRedisplay();
				return;
				break;
			default:
				break;
			}
		default:
			break;
		}

		switch (key) {
		case GLUT_KEY_UP:
			if (last < (int)buffer.size() - 1) last++;
			break;
		case GLUT_KEY_DOWN:
			if (last > 0) last--;
			break;
		default:
			break;
		}

		if (last != 0) {
			int i = 0, j = 0;
			bool found = false;
			if (buffer.size() < 2) {
				return;
			}
			for (i = buffer.size() - 2; i >= 0; i--) {
				if (buffer.type(i) == ConsoleBuffer::sInput) {
					j++;
				}

				if (j == last) {
					buffer.back() = buffer[i];
					found = true;
					break;
				}
			}
			if (!found) {
				//не нашли
				last = 0;
				buffer.back() = BufferItem("");
			}
		} else {
			buffer.back() = BufferItem("");
		}
		glutPostRedisplay();
	}
}

void Viz_Console::pushErrorString(const char *s) {
	pushString(s, ConsoleBuffer::sError);
}

void Viz_Console::pushErrorString(string &s) {
	pushString(s, ConsoleBuffer::sError);
}

void Viz_Console::pushInputString(const char *s) {
	pushString(s, ConsoleBuffer::sInput);
}

void Viz_Console::pushInputString(string &s) {
	pushString(s, ConsoleBuffer::sInput);
}

void Viz_Console::pushOutputString(const char *s) {
	pushString(s, ConsoleBuffer::sOutput);
}

void Viz_Console::pushOutputString(string &s) {
	pushString(s, ConsoleBuffer::sOutput);
}

void Viz_Console::pushString(string &s, int sType) {
	if ((int)buffer.size() >= buffer_max_size) {
		buffer.erase(buffer.begin());
	}

	if (buffer.empty()) {
		buffer.push_back(s, sType);
		buffer.push_back(""); //наверху всегда лежит строка ввода
	} else {
		buffer.back() = BufferItem(s, sType);
		buffer.push_back(""); //наверху всегда лежит строка ввода
	}
}

void Viz_Console::pushString(const char * s, int sType) {
	if ((int)buffer.size() >= buffer_max_size) {
		buffer.erase(buffer.begin());
	}

	if (buffer.empty()) {
		buffer.push_back(s, sType);
		buffer.push_back(""); //наверху всегда лежит строка ввода
	} else {
		buffer.back() = BufferItem(s, sType);
		buffer.push_back(""); //наверху всегда лежит строка ввода
	}
}

void Viz_Console::printCommandsList() {
	string commands []  = {
		"help",
	};

	pushString("Commands List:", ConsoleBuffer::sOutput);
	for (int i = 0; i < 1; i++) {
		pushString(commands[i], ConsoleBuffer::sOutput);
	}

	Viz_Descr::DescrMap & mp = Viz_Descr::Instance().Descr();
	Viz_Descr::DescrMap::iterator it, st = mp.begin(), en = mp.end();
	for (it = st; it != en; ++it) {
		string str = it->first + " : " + it->second;
		pushString(str, ConsoleBuffer::sOutput);
	}
}

void Viz_Console::printHelp() {
	printCommandsList();
	pushString("Help:", ConsoleBuffer::sOutput);
	pushString("Keys:", ConsoleBuffer::sOutput);
	pushString("d - drawing mode", ConsoleBuffer::sOutput);
	pushString("p - switch perspective", ConsoleBuffer::sOutput);
	pushString("+ - zoom in", ConsoleBuffer::sOutput);
	pushString("- - zoom out", ConsoleBuffer::sOutput);
	pushString("<tab> - commands list/autocomplete (in console)", ConsoleBuffer::sOutput);
	pushString("f - fullscreen", ConsoleBuffer::sOutput);
	pushString("q - quit", ConsoleBuffer::sOutput);
	pushString("shift+up/down - scroll console text", ConsoleBuffer::sOutput);
}

void Viz_Console::loadHistory() {
	FILE * f = fopen("viz_history.txt", "r");
	if (!f) { return; }
	const int buf_size = 32768;
	char buf [buf_size];
	while (fgets(buf, buf_size - 1, f) != 0) {
		int len = strlen(buf);
		if (len <= 1) continue;
		string s(buf, len - 1);
		pushInputString(s);	
	}
}

void Viz_Console::save() {
	saveHistory();
}

void Viz_Console::saveHistory() {
	string last_save;
	FILE * f = fopen("viz_history.txt", "w");
	if (!f) throw NotFound("viz_history.txt", __FILE__, __LINE__);
	for (int i = 0; i < (int)buffer.size(); i++) {
		if (buffer.type(i) == ConsoleBuffer::sInput) {
			string str_i = buffer.str(i);
			if (str_i != last_save) {
				fprintf(f, "%s\n", str_i.c_str());
			}
			last_save = str_i;
		}
	}
	fclose(f);
}

BufferItem::~BufferItem() {
}

BufferItem::BufferItem(string &s1, int t) {
	s = s1; 
	stype = t;
}

BufferItem::BufferItem(const char * str, int t) {
	s = string(str); 
	stype = t;
}

//строка
string& BufferItem::str() {
	return s;
}

int BufferItem::type() {
	return stype;
}

//i'я строка
string ConsoleBuffer::str(int i) {
	return at(i).str();
}

//префикс i'й строки
string ConsoleBuffer::pre(int i) {
	return off_symb_map[at(i).type()];
}

//цвета i'й строки
float * ConsoleBuffer::color(int i) {
	int t = type(i);
	return color_map[t];
}

float* ConsoleBuffer::colorOfType(int t) {
	return color_map[t];
}

//тип i'й строки
int ConsoleBuffer::type(int i) {
	return at(i).type();
}

void ConsoleBuffer::push_back(const char *s, int t) {
	BufferItem b(s, t);
	push_back(b);
}

void ConsoleBuffer::push_back(string &s, int t) {
	BufferItem b(s, t);
	push_back(b);
}

void ConsoleBuffer::push_back(BufferItem &b) {
	vector < BufferItem >::push_back(b);
}

string &ConsoleBuffer::backStr() {
    return back().str();
}

void ConsoleBuffer::backAppend(int num, char c) {
    backStr().append(num, c);
}
