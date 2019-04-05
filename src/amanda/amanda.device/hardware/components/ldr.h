#pragma once

#include "abstraction/sensor.h"

class LDR : public AnalogSensor
{
	public: LDR(byte pin) : AnalogSensor(pin) { }
	public: virtual AnalogValue read() const override;
	public: virtual const char* const unit() const { return "Lux"; };
};


