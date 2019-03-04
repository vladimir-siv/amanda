#pragma once

#include "exception.h"

class CollectionFullException : public Exception
{
	public: virtual const char* what() const override
	{
		return "Collection is full.";
	}
};
