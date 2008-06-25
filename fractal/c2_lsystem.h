/*$Id$*/

/* Copyright (c) 2008 Alexey Ozeritsky (Алексей Озерицкий)
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

#ifndef LSYSTEM_H
#define LSYSTEM_H
#include <string>
#include <map>
#include <set>
#include <list>
#include <iostream>

struct Group
{
	bool error;
	double alpha;
	double theta;
	std::string axiom;
	std::map < char, std::string > r;
	std::set < char > v;
	std::list < std::string > mgl; //строки для вывода в mgl
	std::string name;
	int order;  //предпочитаемый порядок отрисовки

	typedef std::map < char, std::string >::iterator iterator;

	Group(): error(false), alpha (0.0), theta (0.0), order(0) {}

	bool check();
	void print();
	void setAxiom(const std::string & a);
	void setOrder(double o) { order = (int)o; }
	void addMgl(const std::string & str) { mgl.push_back(str); }
};

struct Parser
{
	bool error;
	std::list < Group > grp;
	typedef std::list < Group >::iterator iterator;
	Group last;
	std::list < std::string > mgl; //строки для вывода в mgl

	Parser (): error(false) {}

	void push(const std::string & name = "") { 
		last.name = name;
		std::list < std::string > m = mgl;
		m.insert(m.end(), last.mgl.begin(), last.mgl.end());
		m.swap(last.mgl);

		grp.push_back(last);
		last = Group();
	}

	void addMgl(const std::string & str) { mgl.push_back(str); }

	void print();
};

inline void Group::setAxiom(const std::string & a) {
	axiom = a;
	for (uint i = 0; i < a.length(); ++i) {
		if (a[i] != '[' && a[i] != ']' && a[i] != '+' && a[i] != '-') {
			v.insert(a[i]);
		}
	}
}

inline void Parser::print() {
	for (std::list < Group >::iterator it = grp.begin(); it != grp.end(); ++it) {
		it->print();
	}
}

inline bool Group::check() {
	if (axiom.empty()) {
		std::cerr << "fail on axiom \n";
		error = true;
		goto fail;
	}

	if (theta == 0.0) {
		std::cerr << "fail on theta \n";
		error = true;
		goto fail;
	}
		
	/*for (std::set < char >::iterator it = v.begin(); it != v.end(); ++it) {
		if (r.find(*it) == r.end()) {
			std::cerr << "fail on " << *it << "\n";
			error = true;
			goto fail;
		}
		}*/

fail:
	return !error;
}

inline void Group::print() {
	std::cerr << "error : " << error << "\n";
	std::cerr << "alpha : " << alpha << "\n";
	std::cerr << "theta : " << theta << "\n";
	std::cerr << "axiom : " << axiom << "\n";
	
	for (iterator it = r.begin(); it != r.end(); ++it) {
		std::cerr << it->first << " : " << it->second << "\n";
	}
}

int yyparse(Parser * );

std::string lsystem(Group & gr, int level);

struct line {
	double x0;
	double y0;
	double x1;
	double y1;
	int c;
};

std::list < line > turtle(Group & p, const std::string & W);

#endif //LSYSTEM_H
