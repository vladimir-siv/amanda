#pragma once

#include "exception.h"

class SchedulerFullException : public Exception
{
	public: virtual const char* what() const override
	{
		return "Scheduler is full.";
	}
};
