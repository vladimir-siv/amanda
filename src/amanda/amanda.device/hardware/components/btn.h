#pragma once

#include "abstraction/sensor.h"

class BTN : public DigitalSensor
{
	public: explicit BTN(byte pin) : DigitalSensor(pin) { }
	public: virtual const __FlashStringHelper* description() const override { return F("button"); }
};
