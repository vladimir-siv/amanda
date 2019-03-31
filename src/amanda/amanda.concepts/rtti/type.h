#pragma once

#include "typename.h"

using ID = const unsigned long long;

class Type final
{
	friend class TypeFactory;

	private: ID newid() { static unsigned long long id = 0; return (ID)++id; }

	private: ID _id = newid();
	public: ID id() const { return _id; }

	private: const char* _name = "<unknown>";
	public: const char* name() const { return _name; }

	private: Type(const char* name) : _name(name) { }
	private: Type(const Type&) = delete;
	private: Type(Type&&) = delete;
	private: Type& operator=(const Type&) = delete;
	private: Type& operator=(Type&&) = delete;

	public: friend bool operator==(const Type& t1, const Type& t2) { return &t1 == &t2; }
	public: friend bool operator!=(const Type& t1, const Type& t2) { return !(t1 == t2); }
};

class TypeFactory { public: static Type& newtype(const char* name) { return *new Type(name); } };
template <typename T> struct __TypeOf__ { static Type& type; };
template <typename T> Type& __TypeOf__<T>::type = TypeFactory::newtype(type_name(T));

#ifndef __USE_RTTI__
#define __RTTI__ public: virtual Type& getType() const { return __TypeOf__<std::remove_const_t<std::remove_reference_t<decltype(*this)>>>::type; }
#define typeid(var) (var).getType()
#else
#define __RTTI__
#endif
