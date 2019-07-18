#pragma once

#include "typename.h"

using ID = const unsigned long long;

class Type final
{
	template <typename T> friend struct __TypeOf__;

	private: static ID newid() { static unsigned long long id = 0; return (ID)++id; }
	
	public: ID id;
	public: const char* const name;
	
	private: Type(const char* const name = "<unknown>") : id(newid()), name(name) { }
	
	public: friend bool operator==(const Type& t1, const Type& t2) { return t1.id == t2.id; }
	public: friend bool operator!=(const Type& t1, const Type& t2) { return t1.id != t2.id; }
};

template <typename T> struct __TypeOf__ { static Type type; };
template <typename T> Type __TypeOf__<T>::type(type_name(T));

#ifndef __USE_RTTI__
#define __RTTI__ public: virtual Type& getType() const { return __TypeOf__<std::remove_const_t<std::remove_reference_t<decltype(*this)>>>::type; }
#define typeid(var) (var).getType()
#else
#define __RTTI__
#endif
