#ifndef LSYSTEM_H
#define LSYSTEM_H
#include <string>
#include <map>
#include <set>
#include <iostream>

struct Parser
{
	bool error;
	double alpha;
	double theta;
    std::string axiom;
    std::map < char, std::string > r;
	std::set < char > v;

	typedef std::map < char, std::string >::iterator iterator;

	Parser (): error(false), alpha (0.0), theta (0.0) {
	}

	void setAxiom(const std::string & a) {
		axiom = a;
		for (uint i = 0; i < a.length(); ++i) {
			if (a[i] != '[' && a[i] != ']' && a[i] != '+' && a[i] != '-') {
				v.insert(a[i]);
			}
		}
	}

	bool check() {
		if (axiom.empty()) {
			error = true;
			goto fail;
		}

		if (theta == 0.0) {
			error = true;
			goto fail;
		}
		
		for (std::set < char >::iterator it = v.begin(); it != v.end(); ++it) {
			if (r.find(*it) == r.end()) {
				error = true;
				goto fail;
			}
		}

	fail:
		return !error;
	}

	void print() {
		std::cerr << "error : " << error << "\n";
		std::cerr << "alpha : " << alpha << "\n";
		std::cerr << "theta : " << theta << "\n";
		std::cerr << "axiom : " << axiom << "\n";
	
		for (iterator it = r.begin(); it != r.end(); ++it) {
			std::cerr << it->first << " : " << it->second << "\n";
		}
	}
};

int yyparse(Parser * );

#endif //LSYSTEM_H

