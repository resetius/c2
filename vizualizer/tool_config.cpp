/*$Id: tool_config.cpp 2455 2008-05-22 12:41:25Z aozeritsky $*/

/* Copyright (c) 2007, 2008 Alexey Ozeritsky (Алексей Озерицкий)
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
 * 3. The name of the author may not be used to endorse or promote products
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

#include <stdlib.h>
#include <string.h>
#include <string>
#include <list>

#include <glib.h>

#include "tool_config.h"

using namespace std;

static const char * not_init = "not initialized";

struct CmdVal {
	union {
		double d;
		int i;
		const char * s;
		const char * init;
		gboolean b;
	} v;

	Config::CmdValType t;

	CmdVal(Config::CmdValType f): t(f) 
	{
		v.init = not_init;
	}
};

class Config::PImpl {
public:
	GKeyFile * config_;
	string fname_;
	bool rewrite_;

	list < CmdVal > cmds_;
	list < GOptionEntry * > entries_;
	back_insert_iterator < list < CmdVal > > cmds_iter_;
	map < string, CmdVal * > cmd2val_;
	map < string, map < string, string > > opt2val_;

	int argc_;
	char ** argv_;

	GOptionContext *context_;

	PImpl(): config_(g_key_file_new()), rewrite_(false), 
		  cmds_iter_ (cmds_),
		  context_ (g_option_context_new (""))
	{
	}

	void init(const std::string & name, int argc, char ** argv)
	{
		fname_ = name;
		argc_  = argc;
		argv_  = argv;

		add_options_entry("verbose", 'v', "Be verbose", NONE, 0);
		add_options_entry("daemon", 'd', "Daemonize", NONE, 0);
		add_options_entry("config", 'c', "Config", STRING, "file_name");
		add_options_entry("option", 'o', "sets options (format: \"group1:key1=value1,group2:key2=value2,...\")", STRING, "options");
		add_options_entry("version", 'V', "print version", STRING, 0);

		check_args();
		parse_o();

		try {
			fname_ = get_string_cmd("c");
		} catch (ConfigError &) {
			;
		}

		if (!fname_.empty()) {
			GError * error = 0;
			if (!g_key_file_load_from_file(config_, fname_.c_str(),
				G_KEY_FILE_KEEP_COMMENTS, &error))
			{
				printf("Cannot load configuration file %s",
					fname_.c_str());

				rewrite_ = true;
			}
		}
	}

	~PImpl() {
		if (rewrite_) {
			store();
		}
		
		g_option_context_free(context_);
		g_key_file_free(config_);

		for (list < GOptionEntry * > ::iterator it = entries_.begin();
			it != entries_.end(); ++it)
		{
			delete *it;
		}
	}

	void check_args()
	{
		GError *error = NULL;
		if (argc_ > 0) {
			//g_option_context_parse меняет argv!!
			char ** argv = new char*[argc_];
			for (int i = 0; i < argc_; ++i) {
				argv[i] = strdup(argv_[i]);
			}

			if (!g_option_context_parse (context_, &argc_, 
				&argv, &error))
			{
				usage();
			}

			for (int i = 0; i < argc_; ++i) {
				free(argv[i]);
			}

			delete [] argv;
		}
	}

	void usage()
	{
		printf("%s\n", g_option_context_get_help (context_, 1, 0));
		exit(-1);
	}

	void add_options_entry(const char * long_name, 
		char  short_name, 
		const char * doc, 
		CmdValType vt, const char * arg_desc)
	{
		GOptionArg type;
		switch (vt) {
		case NONE:
			type = G_OPTION_ARG_NONE;
			break;
		case INT:
			type = G_OPTION_ARG_INT;
			break;
		case STRING:
			type = G_OPTION_ARG_STRING;
			break;
		case DOUBLE:
			type = G_OPTION_ARG_DOUBLE;
			break;
		};

		*cmds_iter_ ++ = CmdVal(vt);

		CmdVal * val = &(*cmds_.rbegin());
		cmd2val_[long_name]          = val;

		string s; s += short_name;
		cmd2val_[s] = val;

		GOptionEntry entries[] = { 
			{ long_name, short_name, 0, type, &val->v, doc, arg_desc },
			{ NULL }
		};

		g_option_context_add_main_entries (context_, entries, 0);
	}

	void store() {
		if (!fname_.empty()) {
			FILE * f = fopen(fname_.c_str(), "w");
			if (f) {
				GError * error = 0;
				gsize len;
				char * data = g_key_file_to_data(config_, &len, &error);
				if (!error) {
					fwrite(data, 1, len, f); 
				}
				fclose(f);
			} else {
				printf("cannot save config %s", fname_.c_str());
			}
		}
	}

	void set_value(const string & key, const string & group, int val)
	{
		g_key_file_set_integer(config_, group.c_str(), key.c_str(), val);
		rewrite_ = true;
	}

	void set_value(const string & key, const string & group, const string & val)
	{
		g_key_file_set_string(config_, group.c_str(), key.c_str(), val.c_str());
		rewrite_ = true;
	}

	void set_value(const string & key, const string & group, double val)
	{
		g_key_file_set_double(config_, group.c_str(), key.c_str(), val);
		rewrite_ = true;
	}

	int get_int_cmd(const string & key)
	{
		if (cmd2val_.find(key) != cmd2val_.end()) {
			CmdVal & v = *cmd2val_[key];
			if ((v.t == INT ||  v.t == NONE) && v.v.init != not_init) {
				return v.v.i;
			}
		}

		throw ConfigError();
	}

	double get_double_cmd(const string & key)
	{
		if (cmd2val_.find(key) != cmd2val_.end()) {
			CmdVal & v = *cmd2val_[key];
			if (v.t == DOUBLE && v.v.init != not_init) {
				return v.v.d;
			}
		}

		throw ConfigError();
	}

	string get_string_cmd(const string & key)
	{
		if (cmd2val_.find(key) != cmd2val_.end()) {
			CmdVal & v = *cmd2val_[key];
			if (v.t == STRING && v.v.s && v.v.init != not_init) {
				return v.v.s;
			}
		}

		throw ConfigError();
	}

	void parse_o()
	{
		try {
			string o = get_string_cmd("o");
			const char * delim = ";, ";

			for (char * token = strtok((char*)o.c_str(), delim); 
				token; token = strtok(0, delim)) 
			{
				char grp[256];
				char key[256];
				char val[1025];
				char * tok = token;
				while (*tok) {
					if (*tok == ':' || *tok == '=') {
						*tok = ' ';
					}
					++tok;
				}

				if (sscanf(token, "%255s%255s%1024s", grp, key, val) == 3) {
					opt2val_[grp][key] = val;
				}
			}
		} catch (ConfigError & ) {
			;
		}
	}

	int get_int_opt2val(const string & key, const string & group) {
		if (opt2val_.find(group) != opt2val_.end()) {
			if (opt2val_[group].find(key) != opt2val_[group].end()) {
				int ret;
				if (sscanf(opt2val_[group][key].c_str(), "%d", &ret) == 1) {
					return ret;
				}
			}
		}

		throw ConfigError();
	}

	double get_double_opt2val(const string & key, const string & group) {
		if (opt2val_.find(group) != opt2val_.end()) {
			if (opt2val_[group].find(key) != opt2val_[group].end()) {
				double ret;
				if (sscanf(opt2val_[group][key].c_str(), "%lf", &ret) == 1) {
					return ret;
				}
			}
		}

		throw ConfigError();
	}

	string get_string_opt2val(const string & key, const string & group) {
		if (opt2val_.find(group) != opt2val_.end()) {
			if (opt2val_[group].find(key) != opt2val_[group].end()) {
				return opt2val_[group][key];
			}
		}

		throw ConfigError();
	}
};

Config::Config(const string & name, int argc, char ** argv): impl_(new PImpl)
{
	init(name, argc, argv);
}

Config::Config(): impl_(new PImpl) {}

void Config::init(const string & name, int argc, char ** argv) 
{
	impl_->init(name, argc, argv);
}

void Config::init(const std::string & fname)
{
	impl_->init(fname, 0, 0);
}

void Config::init(int argc, char ** argv)
{
	impl_->init("", argc, argv);
}

Config::~Config() {
	delete impl_;
}

void Config::rewrite() const {
	if (!impl_) {
		printf("cannot sync, first use setName() method!");
	}
	
	if (impl_->rewrite_) {
		impl_->store();
	}
}

void Config::reread() {
	if (!impl_) {
		printf("cannot reread, first use setName() method!");
	}

	if (impl_->rewrite_) {
		printf("configuration changed in memory");
		printf("saving: ... ");
		impl_->store();
		impl_->rewrite_ = false;
	} else {
		GError * error = 0;
		printf("reloading configuration");
		g_key_file_free(impl_->config_);
		impl_->config_ = g_key_file_new();
		if (!g_key_file_load_from_file(impl_->config_, impl_->fname_.c_str(),
		                               G_KEY_FILE_KEEP_COMMENTS, &error))
		{
			printf("Cannot load configuration file %s ",
			                      impl_->fname_.c_str());

			impl_->rewrite_ = true;
		}
	}
}

int Config::value(const std::string & key,
                  const std::string & group,
                  int default_value)
{
	if (!impl_) {
		printf("cannot getValue, first use init() method!");
		return default_value;
	}

	try {
		return getInt(key, group);
	} catch (ConfigError & ) {
		impl_->set_value(key, group, default_value);
		return default_value;
	}
}

int Config::getInt(const std::string & key,
                   const std::string & group) const
{
	if (!impl_) {
		throw ConfigError();
	}

	if (group == "cmd") {
		return impl_->get_int_cmd(key);
	}

	try {
		return impl_->get_int_opt2val(key, group);
	} catch (ConfigError & ) {
		;
	}

	int val;
	GError * error = 0;
	if ((val =
	     g_key_file_get_integer(impl_->config_, group.c_str(),
	                            key.c_str(), &error)) == 0)
	{
		if (error) {
			throw ConfigError();
		} else {
			return val;
		}
	} else {
		return val;
	}
}

string Config::value(const std::string & key,
                     const std::string & group, 
                     const char * default_value)
{
	if (!impl_) {
		printf("cannot getValue, first use init() method!");
		return default_value;
	}

	try {
		return getString(key, group);
	} catch (ConfigError & ) {
		impl_->set_value(key, group, default_value);
		return default_value;
	}
}

string Config::getString(const std::string & key,
                         const std::string & group) const
{
	if (!impl_) {
		throw ConfigError();
	}

	if (group == "cmd") {
		return impl_->get_string_cmd(key);
	}

	try {
		return impl_->get_string_opt2val(key, group);
	} catch (ConfigError & ) {
		;
	}

	char * val;
	GError * error = 0;
	if ((val =
	     g_key_file_get_string(impl_->config_, group.c_str(),
	                           key.c_str(), &error)) == 0)
	{
		throw ConfigError();
	} else {
		string ret = val;
		free(val);
		return ret;
	}
}

double Config::value(const std::string & key,
                     const std::string & group,
                     double default_value)
{
	if (!impl_) {
		printf("cannot getValue, first use init() method!");
		return default_value;
	}

	try {
		return getDouble(key, group);
	} catch (ConfigError & ) {
		impl_->set_value(key, group, default_value);
		return default_value;
	}
}

double Config::getDouble(const std::string & key,
                         const std::string & group) const
{
	if (!impl_) {
		throw ConfigError();
	}

	if (group == "cmd") {
		return impl_->get_double_cmd(key);
	}

	try {
		return impl_->get_double_opt2val(key, group);
	} catch (ConfigError & ) {
		;
	}

	double val;
	GError * error = 0;
	if ((val =
	     g_key_file_get_double(impl_->config_, group.c_str(),
	                           key.c_str(), &error)) == 0)
	{
		if (error) {
			throw ConfigError();
		} else {
			return val;
		}
	} else {
		return val;
	}
}

void Config::usage()
{
	impl_->usage();
}

void Config::addAlias(const char * long_name, 
					  char  short_name, 
					  CmdValType vt,
					  const char * doc,
					  const char * arg_desc)
{
	impl_->add_options_entry(long_name, short_name, doc, vt, arg_desc);
	impl_->check_args();
}
