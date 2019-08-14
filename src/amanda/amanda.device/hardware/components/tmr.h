#pragma once

#include <Arduino.h>
#include <system.h>
#include "abstraction/sensor.h"

class TMR final : public AnalogSensor
{
	public: static TMR& instance() { static TMR timer; return timer; }
	
	private: explicit TMR() : AnalogSensor(0) { }
	public: virtual const __FlashStringHelper* description() const override { return F("timer"); }
	
	public: virtual AnalogValue read() const override { return System::millis(); }
	public: virtual unit s_unit() const override { char lbl[] { 'm', 's', 0 }; unit u(lbl); return u; }
};
