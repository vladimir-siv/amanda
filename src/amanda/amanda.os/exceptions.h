#pragma once

#include "patterns/singleton.h"
#include "exceptions/exception.h"
#include "exceptions/insufficientstacksizeexception.h"
#include "exceptions/schedulerfullexception.h"

#define MAX_EXCEPTIONS 3

namespace Exceptions
{
	void Throw(Exception* ex);
	Exception* Fetch();
	Exception* Check();
	unsigned int Count();
	// Singleton of Exception type :: Instance()
	template<class T> void Throw() { Throw(&S<T>::I()); }
}
