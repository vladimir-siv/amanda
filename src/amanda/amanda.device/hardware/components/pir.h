#pragma once

#include "abstraction/sensor.h"

#include <system.h>

class PIR : public DigitalSensor
{
	protected: bool _detected = false;
	protected: Time _last = 0;
	
	protected: byte measure() { byte percentage = 0; for (byte i = 0; i < 100; ++i) percentage += DigitalSensor::read(); return percentage; }
	protected: bool detect()
	{
		byte percentage = measure();
		if (!_detected && percentage >= 75) _detected = true, _last = System::millis();
		else if (_detected && percentage <= 10 && System::millis() - _last >= 1000) _detected = false; // _last >= 5000
		return _detected;
	}
	
	public: explicit PIR(byte pin) : DigitalSensor(pin) { }
	public: virtual const char* description() const override { return "PIR"; }
	
	public: virtual DigitalState read() const override { return (DigitalState)const_cast<PIR* const>(this)->detect(); }
};
