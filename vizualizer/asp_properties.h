#ifndef _PROP_H
#define _PROP_H

/*  $Id: asp_properties.h 1776 2007-09-11 13:05:34Z manwe $  */

/* Copyright (c) 2004, 2005 Alexey Ozeritsky
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
 * Парсинг конфигов.
 * класс Properties аналог такого же из java api
 */

#include <stdio.h>
#include <string>
#include <map>
#include <set>
#include <vector>

class Properties {
public:
	Properties(Properties &p);
	Properties();
	Properties(const char fname[]);
	~Properties();
	std::string getProperty(const char * key, const char * def);
	void setProperty(const char * key, const char * val);
	void store();
	void list();
	void list(FILE *f);
	void list_to_log();

protected:
	std::string fname;
	std::map <std::string, std::string> pmap;
	std::set <std::string> new_opt;
	std::map <int, std::string > comments;	
};

/**
 * Сохранение конфигурации в файле
 */
class Conf {
public:
	Conf(Conf&);
	Conf():
		p(0)
	{
	}

	virtual ~Conf();
	Conf(const char fname[]);
	double loadDouble(const char* key, double def);
	int loadInt(const char* key, int def);
	std::string loadString(const char* key, const char * def);
	void loadVector(double *v, const char * key, double * def, int n);
	void save();
	void print() {p->list(); }
	void print(FILE * f) {p->list(f); }

protected:
	Properties *p;
};

#endif //_PROP_H
