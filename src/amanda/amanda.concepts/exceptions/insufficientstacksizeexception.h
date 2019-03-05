#pragma once

#include "exception.h"

class InsufficientStackSizeException : public Exception
{
	public: virtual const char* what() const override
	{
		return "Stack size is too small.";
	}
};
