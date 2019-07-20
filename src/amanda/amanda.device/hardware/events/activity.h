#pragma once

#pragma once

#include <Arduino.h>

#include <dependency.h>

class activity final
{
	private: mutable sdd::type<bool> req;
	private: mutable sdd::type<void*> unused;
	
	public: activity() :
		req(false),
		unused(nullptr)
	{ }
	
	public: bool setReq(bool value) const
	{
		if (req.real != value)
		{
			req.real = value;
			if (value) raise();
			else expire();
		}

		return value;
	}
	
	private: void raise() const
	{
		digitalWrite(13, HIGH);
	}
	private: void expire() const
	{
		digitalWrite(13, LOW);
	}
};
