#ifndef TOOL_CONFIG_H
#define TOOL_CONFIG_H
/*$Id: tool_config.h 2455 2008-05-22 12:41:25Z aozeritsky $*/

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

#include <string>
#include <stdexcept>

class Config {
	class PImpl;
	PImpl * impl_;

public:
	//value type
	enum CmdValType {
		NONE   = 0,
		INT    = 1,
		STRING = 2,
		DOUBLE = 3,
	};

	class ConfigError: public std::logic_error
	{
	public:
		ConfigError(): std::logic_error("configuration error")
		{
		}

		virtual ~ConfigError() {}
	};

	Config(const std::string & fname, int argc = 0, char ** argv = 0);

	Config();
	~Config();

//@{ функции возвращают default_value если ключ в группе не найден
	int value(const std::string & key,
	          const std::string & group,
	          int default_value);

	double value(const std::string & key,
	             const std::string & group,
	             double default_value);

	std::string value(const std::string & key,
	                  const std::string & group,
	                  const char * default_value);
//@}

//@{ функции кидают исключение если ключ в группе не найден
	int getInt(const std::string & key,
	           const std::string & group) const;

	double getDouble(const std::string & key,
	                 const std::string & group) const;

	std::string getString(const std::string & key,
	                      const std::string & group) const;
//@}

	void usage();

	void init(const std::string & fname);
	void init(int argc, char ** argv);
	void init(const std::string & fname, int argc, char ** argv);

	void reread();        //!<перечитывает конфиг
	void rewrite() const; //!<сохраняет на диск (если были изменения в памяти)
};

#include "tool_factory.h"
#endif //TOOL_CONFIG_H
