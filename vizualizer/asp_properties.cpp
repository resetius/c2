/*$Id: asp_properties.cpp 1776 2007-09-11 13:05:34Z manwe $*/

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
 * Поддержка простых файлов конфигурации
 */
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#ifdef _MPI_BUILD
#include <mpi.h>
#endif

#include "asp_excs.h"
#include "asp_singleton.h"
#include "asp_properties.h"

using namespace std;

#ifdef _WIN32
#define snprintf _snprintf
#endif

Properties::Properties(Properties &p) 
{
	pmap     = p.pmap;
	comments = p.comments;
	fname    = p.fname;
}

Properties::Properties(const char *fname1)
{
	fname = fname1;
	FILE *f = 0;

		f = fopen(fname.c_str(),"r");

	map < string, string> &p = pmap;

	if (!f) return; //throw NotFound(fname, __FILE__, __LINE__);
	
	char c;
	string key;
	string val;
#define buf_size 32768
	char buf [buf_size];
	vector < string > strs;
	int c_strs = 0;

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

	if (f) {fclose(f); f = 0;}

		c_strs = strs.size();

	for (int j = 0; j < c_strs; j++) {
		string &str = strs[j];
		int i   = 0;
		int len = str.length();
		
		if (len < 2) continue;
		if (str[0] == '#' || (str[0] == '/' && str[1] == '/')) {
			comments[j] = str;
			continue;
		}

		key.clear(); val.clear();
		while (i < len) {
			c = str[i];
			if (c == '=') {
				//read value
				i++;
				while (i < len) {
					c = str[i];
					//if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
					//if (isspace(c)) {
					if (c == '\n' || c == '\r') {
						break;
					}
					val += c;
					i++;
				}
				p[key] = val;
				break;		
			} else {
				if (isalpha(c) || (key.length() > 0 && isdigit(c))) {
					key += c;
				}
				i++;
			}
		}
	}
}

Properties::Properties()
{	
}

Properties::~Properties() {
}

string Properties::getProperty(const char *key, const char * def) {
	map < string, string> &p = pmap;
	map<string,string>::iterator f = p.find(string(key));
	if (f != p.end()) {
		//sprintf(ret,"%s",(f->second).c_str());
		return f->second;
	} else {
		//sprintf(ret,"%s",def);
		new_opt.insert(key);
		return string(def);
	}
	//return ret;
}

void Properties::setProperty(const char * key, const char * val) {
	map < string, string> &p = pmap;
	p[string(key)]=val;
}

void Properties::store() {
	if (new_opt.empty())
		return; //писать нечего
	FILE *f = fopen(fname.c_str(), "a"); 
	if (!f) {return ; }
	//int i = 0;
	map < string, string> &p = pmap;
	typedef set<string>::const_iterator CI;
	CI i, s = new_opt.begin(), e = new_opt.end();
	for (i = s; i != e; ++i) {
		const string & key   = *i;
		const string & value = pmap[key];
		if (!key.empty() && !value.empty()) {
			fprintf(f, "%s=%s\n", key.c_str(), value.c_str());
		}
	}

	fclose(f);
}

void Properties::list() {
	list(stderr);
}

void Properties::list(FILE *f) {
	map < string, string> &p = pmap;
	typedef map<string,string>::const_iterator CI;
	for (CI c = p.begin(); c != p.end(); ++c) {
		if(!c->first.empty()&&!c->second.empty())
			fprintf(f,"%s=%s\n",(c->first).c_str(),(c->second).c_str());
	}
}

Conf::Conf(Conf&conf) {
	p   = new Properties(*conf.p);

}

Conf::Conf(const char fname[]

) {

	p = new Properties(fname);

}

Conf::~Conf() {
	delete p;
}

double Conf::loadDouble(const char* key, double def) {
	double ret=0.0;
	char str[256];
	snprintf(str, 255, "%.16lf",def);
	//printf("::%s:%s\n",key,p->getProperty(key,str));
	sscanf(p->getProperty(key,str).c_str(),"%lf",&ret);

	snprintf(str, 255, "%.16lf",ret);
    p->setProperty(key,str);
    return ret;
}

int Conf::loadInt(const char* key, int def) {
	int ret=0;
	char str[256];
	snprintf(str, 255, "%d", def);
	sscanf(p->getProperty(key,str).c_str(),"%d",&ret);

	snprintf(str, 255, "%d", ret);
    p->setProperty(key,str);
    return ret;
}

void Conf::loadVector(double *v, const char * key, double * def, int n) {
	char str_i[256];
	string str;
	//char ret[256];
	bool free_def = false;
	if (def == 0) {
		def = new double[n];
		memset(def, 0, n * sizeof(double));
		free_def = true;
	}

	for (int i = 0; i < n; i++) {
		if (i != n - 1) {
			snprintf(str_i, 255, "%.16lf,", def[i]);
		} else {
			snprintf(str_i, 255, "%.16lf", def[i]);
		}
		str.append(str_i);
	}
	//sscanf(p->getProperty(key, str), "%s", ret);
	//string s_ret(ret);
	string s_ret = p->getProperty(key, str.c_str());

	int s = 0;
	int s1 = 0;
	int j = 0;
	while (s1 != (int)s_ret.length() - 1) {
		s1 = s_ret.find(',', s);
		if (s1 < 0) s1 = s_ret.length() - 1;
		if (s1 > 0) {
			string v_el = s_ret.substr(s, s1 - s);
			if (j > n) break;
			sscanf(v_el.c_str(), "%lf", &v[j]);
			j++; s = s1 + 1;
		}
	}
	if (j < n - 1) {
		//memcpy(v, def, n * sizeof(double));
		for (int i = j; i < n ; i++) {
			v[i] = def[i];
		}
	}

	str.clear();
	for (int i = 0; i < n; i++) {
		if (i != n - 1) {
			snprintf(str_i, 255, "%.16lf,", v[i]);
		} else {
			snprintf(str_i, 255, "%.16lf", v[i]);
		}
		str.append(str_i);
	}
	p->setProperty(key, str.c_str());

	if (free_def) delete [] def;
}

string Conf::loadString(const char* key, const char * def) {
	string ret;
	//char s[256];
	//sscanf(p->getProperty(key,def),"%255s",s);
	//ret = s;
	ret = p->getProperty(key,def);
	p->setProperty(key, ret.c_str());
	return ret;
}

void Conf::save() {
	p->store();
}
