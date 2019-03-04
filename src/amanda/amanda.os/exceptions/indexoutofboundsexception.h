#pragma once

#include "exception.h"

class IndexOutOfBoundsException : public Exception
{
	public: virtual const char* what() const override
	{
		return "Index is out of bounds.";
	}
};
