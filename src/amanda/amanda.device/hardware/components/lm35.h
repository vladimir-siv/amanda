#pragma once

#include "abstraction/sensor.h"

#include <system.h>
#include <thread.h>

class LM35 : public AnalogSensor
{
	protected: AnalogValue temperature = 0;
	protected: Time lts = 0;
	protected: AnalogValue measure()
	{
		if ((System::millis() - lts >= 60000) || (lts == 0))
		{
			temperature = 0;
			
			for (byte i = 0; i < 30; ++i)
			{
				temperature += AnalogSensor::read();
				Thread::sleep(1);
			}

			temperature /= 30.0;
			lts = System::millis();
		}

		return temperature;
	}
	
	public: LM35(byte pin) : AnalogSensor(pin) { }
	public: virtual AnalogValue read() const { return const_cast<LM35* const>(this)->measure() / 2.046 /*2.15*/; }
	public: virtual const char* const unit() const override { return "*C"/*"°C"*/; };
};
