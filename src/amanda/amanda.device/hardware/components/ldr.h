#pragma once

#include "abstraction/sensor.h"

class LDR : public AnalogSensor
{
	public: explicit LDR(byte pin) : AnalogSensor(pin) { }
	public: virtual const __FlashStringHelper* description() const override { return F("LDR"); }
	
	public: virtual AnalogValue read() const override;
	public: virtual unit s_unit() const override { char lbl[] { 'l', 'u', 'x', 0 }; unit u(lbl); return u; }
};
