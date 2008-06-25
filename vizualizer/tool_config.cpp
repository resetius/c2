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

#include <glib.h>

#include "tool_config.h"

using namespace std;

class Config::PImpl {
public:
	GKeyFile * config_;
	string fname_;
	bool rewrite_;

	PImpl(const std::string & name)
	      : config_(g_key_file_new()), fname_(name), rewrite_(false) 
	{
		GError * error = 0;
		if (!g_key_file_load_from_file(config_, fname_.c_str(),
		                               G_KEY_FILE_KEEP_COMMENTS, &error))
		{
			printf("Cannot load configuration file %s",
			                      fname_.c_str());

			rewrite_ = true;
		}
	}

	~PImpl() {
		if (rewrite_) {
			store();
		}
		
		g_key_file_free(config_);
	}

	void store() {
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
};

Config::Config(const string & name): impl_(new PImpl(name)) {
}

Config::Config(): impl_(0) {}

void Config::setName(const string & name) {
	delete impl_;
	impl_ = new PImpl(name);
}

Config::~Config() {
	delete impl_;
}

void Config::sync() {
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

int Config::getValue(const std::string & key,
                     const std::string & group,
                     int default_value)
{
	if (!impl_) {
		printf("cannot getValue, first use setName() method!");
		return default_value;
	}

	int val;
	GError * error = 0;
	if ((val =
	     g_key_file_get_integer(impl_->config_, group.c_str(),
	                            key.c_str(), &error)) == 0)
	{
		if (error) {
			g_key_file_set_integer(impl_->config_, group.c_str(),
			                       key.c_str(), default_value);
			impl_->rewrite_ = true;
			return default_value;
		} else {
			return val;
		}
	} else {
		return val;
	}
}

string Config::getValue(const std::string & key,
                        const std::string & group, 
                        const char * default_value)
{
	if (!impl_) {
		printf("cannot getValue, first use setName() method!");
		return default_value;
	}

	char * val;
	GError * error = 0;
	if ((val =
	     g_key_file_get_string(impl_->config_, group.c_str(),
	                           key.c_str(), &error)) == 0)
	{
		g_key_file_set_string(impl_->config_, group.c_str(),
		                      key.c_str(), default_value);
		impl_->rewrite_ = true;
		return default_value;
	} else {
		string ret = val;
		free(val);
		return ret;
	}
}

double Config::getValue(const std::string & key,
                        const std::string & group,
                        double default_value)
{
	if (!impl_) {
		printf("cannot getValue, first use setName() method!");
		return default_value;
	}

	double val;
	GError * error = 0;
	if ((val =
	     g_key_file_get_double(impl_->config_, group.c_str(),
	                           key.c_str(), &error)) == 0)
	{
		if (error) {
			g_key_file_set_double(impl_->config_, group.c_str(),
			                      key.c_str(), default_value);
			impl_->rewrite_ = true;
			return default_value;
		} else {
			return val;
		}
	} else {
		return val;
	}
}
