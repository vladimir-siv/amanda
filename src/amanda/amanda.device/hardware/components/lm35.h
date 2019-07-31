#pragma once

#include "abstraction/sensor.h"

#include "../../common/units.h"

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
				Thread::delay(1);
			}

			temperature /= 30.0;
			lts = System::millis();
		}

		return temperature;
	}
	
	public: explicit LM35(byte pin) : AnalogSensor(pin) { }
	public: virtual const __FlashStringHelper* description() const override { return F("LM35"); }
	
	public: virtual AnalogValue read() const { return const_cast<LM35* const>(this)->measure() / unitconstants::celsius(); }
	public: virtual unit s_unit() const override { char lbl[] { '*', 'C', 0 }; unit u(lbl); return u; }
};
