#pragma once

#include "abstraction/sensor.h"

class BTN : public DigitalSensor
{
	public: BTN(byte pin) : DigitalSensor(pin) { }
};
