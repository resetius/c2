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
	std::set < std::string > mgl; //строки для вывода в mgl
	std::string name;
	int order;  //предпочитаемый порядок отрисовки

	typedef std::map < char, std::string >::iterator iterator;

	Group(): error(false), alpha (0.0), theta (0.0), order(0) {}

	bool check();
	void print();
	void setAxiom(const std::string & a);
	void setOrder(double o) { order = (int)o; }
	void addMgl(const std::string & str) { mgl.insert(str); }
};

struct Parser
{
	bool error;
	std::list < Group > grp;
	typedef std::list < Group >::iterator iterator;

	Group last;

	Parser (): error(false) {}

	void push(const std::string & name = "") { 
		last.name = name;
		grp.push_back(last);
		last = Group();
	}

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
};

std::list < line > turtle(Group & p, const std::string & W);

#endif //LSYSTEM_H
