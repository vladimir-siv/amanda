#pragma once

#include "abstraction/sensor.h"

class POT : public AnalogSensor
{
	public: explicit POT(byte pin) : AnalogSensor(pin) { }
	public: virtual const __FlashStringHelper* description() const override { return F("potentiometer"); }
	
	public: virtual AnalogValue read() const { return AnalogSensor::read() / 10.23; }
	public: virtual unit s_unit() const override { char lbl[] { '%', 0 }; unit u(lbl); return u; }
};
