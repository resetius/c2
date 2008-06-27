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

union CmdVal {
	double d;
	int i;
	char * s;
	gboolean b;
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

	int argc_;
	char ** argv_;

	GOptionContext *context_;

	PImpl(const std::string & name, int argc, char ** argv)
	      : config_(g_key_file_new()), fname_(name), rewrite_(false), 
		  cmds_iter_ (cmds_)
	{
		if (fname_.empty()) {
			GError * error = 0;
			if (!g_key_file_load_from_file(config_, fname_.c_str(),
				G_KEY_FILE_KEEP_COMMENTS, &error))
			{
				printf("Cannot load configuration file %s",
					fname_.c_str());

				rewrite_ = true;
			}
		}

		argc_ = argc;
		argv_ = argv;

		context_ = g_option_context_new ("");

		add_options_entry("verbose", 'v', "Be verbose", NONE, 0);
		add_options_entry("daemon", 'd', "Daemonize", NONE, 0);
		add_options_entry("config", 'c', "Config", STRING, "file_name");
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

		*cmds_iter_ ++ = CmdVal();

		CmdVal * val = &(*cmds_.rbegin());
		cmd2val_[long_name]          = val;

		string s; s += short_name;
		cmd2val_[s] = val;

		/*GOptionEntry * ent = new GOptionEntry();
		ent->long_name   = long_name;
		ent->short_name  = short_name;
		ent->flags       = 0;
		ent->arg         = type;
		ent->arg_data    = (void*)val;
		ent->description = doc;
		ent->arg_description = arg_desc;*/

		GOptionEntry entries[] = { 
			{ long_name, short_name, 0, type, val, doc, arg_desc },
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
};

Config::Config(const string & name, int argc, char ** argv): impl_(0) 
{
	init(name, argc, argv);
}

Config::Config(): impl_(0) {}

void Config::init(const string & name, int argc, char ** argv) 
{
	delete impl_;
	impl_ = new PImpl(name, argc, argv);

	usage();
}

void Config::init(const std::string & fname)
{
	init(fname, 0, 0);
}

void Config::init(int argc, char ** argv)
{
	init("", argc, argv);
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

void Config::check()
{
	GError *error = NULL;
	if (impl_->argc_ > 0)
	if (!g_option_context_parse (impl_->context_, &impl_->argc_, 
		&impl_->argv_, &error))
	{
		throw ConfigError();
	}
}

void Config::usage()
{
	check();
	printf("%s\n", g_option_context_get_help (impl_->context_, 1, 0));
	exit(-1);
}
