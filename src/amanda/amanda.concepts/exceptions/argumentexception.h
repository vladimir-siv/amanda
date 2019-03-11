#pragma once

#include "exception.h"

class ArgumentException : public Exception
{
	public: virtual const char* what() const override
	{
		return "Argument value is not valid.";
	}
};
