#pragma once

#include "exception.h"

class CollectionEmptyException : public Exception
{
	public: virtual const char* what() const override
	{
		return "Collection is empty.";
	}
};
