#pragma once

#include "abstraction/sensor.h"

class POT : public AnalogSensor
{
	public: POT(byte pin) : AnalogSensor(pin) { }
	public: virtual AnalogValue read() const { return AnalogSensor::read() / 10.23; }
	public: virtual const char* const unit() const override { return "%"; };
};
