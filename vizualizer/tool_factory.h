#ifndef _REVIDX_FACTORY_H
#define _REVIDX_FACTORY_H
/*$Id$*/

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

#include <exception>
#include <sstream>
#include <vector>
#include <map>

template < typename IdentifierType, typename ProductType >
class DefaultFactoryError {
public:
	class Exception: public std::exception {
	public:
		Exception(const IdentifierType & id) : unknownId_(id) {
		}

		~Exception() throw() {
		}

		virtual const char * what() const throw() {
			std::ostringstream ostr;
			ostr << "Unknown type : " << unknownId_;
			what_ = ostr.str();
			return what_.c_str();
		}

		const IdentifierType GetId() {
			return unknownId_;
		}

	private:
		mutable std::string what_;
		IdentifierType unknownId_;
	};

protected:
	static ProductType * OnUnknownType(const IdentifierType & id) {
		throw Exception(id);
	}
};

template <
	typename AbstractProduct,
	typename IdentifierType,
	typename ProductCreator,
	template <typename, class> class FactoryErrorPolicy = DefaultFactoryError
>
class Factory: public FactoryErrorPolicy < IdentifierType, AbstractProduct >
{
public:
	bool Register(const IdentifierType & id, ProductCreator creator) {
		return assiciations_.insert(typename AssocMap::value_type(id, creator)).second;
	}

	bool Unregister(const IdentifierType & id) {
		return assiciations_.erase(id) == 1;
	}

	AbstractProduct * CreateObject(const IdentifierType & id) {
		typename AssocMap::const_iterator i = assiciations_.find(id);
		if (i != assiciations_.end()) {
			return (i->second)();
		}
		return FactoryErrorPolicy < IdentifierType, AbstractProduct >::OnUnknownType(id);
	}

protected:
	typedef std::map<IdentifierType, ProductCreator> AssocMap;
	AssocMap assiciations_;
};

template <
	typename AbstractProduct,
	typename ProductCreator,
	template <typename, class> class FactoryErrorPolicy
>
class Factory < AbstractProduct, unsigned char, ProductCreator, FactoryErrorPolicy >
	: public FactoryErrorPolicy < unsigned char, AbstractProduct >
{
public:
	Factory() {
		assiciations_.resize(255);
	}
	
	bool Register(unsigned char id, ProductCreator creator) {
		assiciations_[id] = creator;
		return true;
	}

	bool Unregister(unsigned char id) {
		assiciations_[id] = 0;
		return true;
	}

	AbstractProduct * CreateObject(unsigned char id) {
		ProductCreator & creator = assiciations_[id];
		if (creator != 0) {
			return (creator)();
		}
		return FactoryErrorPolicy < unsigned char, AbstractProduct >::OnUnknownType(id);
	}

protected:
	typedef std::vector< ProductCreator > AssocMap;
	AssocMap assiciations_;
};

template < typename T >
class Singleton {
public:
	static T & Instance() {
		static T t;
		return t;
	}

	template < typename Prm1 >
	static T & Instance(Prm1 & prm1) {
		static T t(prm1);
		return t;
	}

private:
	~Singleton() {};
	Singleton() {};
};

#endif //_REVIDX_FACTORY_H

