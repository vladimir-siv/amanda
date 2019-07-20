#pragma once

#include <Arduino.h>
#include <system.h>
#include "abstraction/sensor.h"

class TMR final : public AnalogSensor
{
	public: static TMR& instance() { static TMR timer; return timer; }
	
	private: explicit TMR() : AnalogSensor(0) { }
	public: virtual const char* description() const override { return "timer"; }
	
	public: virtual AnalogValue read() const { return System::millis(); }
	public: virtual const char* const unit() const override { return "ms"; };
};
