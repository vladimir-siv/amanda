#pragma once

#include "patterns/singleton.h"
#include "exceptions/exception.h"
#include "exceptions/insufficientstacksizeexception.h"
#include "exceptions/collectionfullexception.h"
#include "exceptions/collectionemptyexception.h"
#include "exceptions/indexoutofboundsexception.h"
#include "exceptions/argumentexception.h"

#define MAX_EXCEPTIONS 3

namespace Exceptions
{
	void Throw(Exception* ex);
	Exception* Fetch();
	Exception* Check();
	unsigned int Count();
	void Clear();
	template <class T> void LThrow()
	{
		Throw(&S<T>::I());
	}
	template <class T> void Throw()
	{
		LThrow<T>();
#ifdef __THROW_EX__
		throw S<T>::I();
#endif
	}
}
