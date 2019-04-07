#pragma once

#include "abstraction/sensor.h"

class LDR : public AnalogSensor
{
	public: explicit LDR(byte pin) : AnalogSensor(pin) { }
	public: virtual const char* description() const override { return "LDR"; }
	
	public: virtual AnalogValue read() const override;
	public: virtual const char* const unit() const { return "Lux"; };
};


