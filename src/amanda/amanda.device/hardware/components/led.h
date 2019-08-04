#pragma once

#include "abstraction/element.h"
#include "functions/toggle.h"

class LED : public DigitalElement, public Toggleable
{
	protected: DigitalState toggle_val = LOW;
	
	public: explicit LED(byte pin) : DigitalElement(pin) { }
	public: virtual const __FlashStringHelper* description() const override { return F("LED"); }
	
	public: virtual const __FlashStringHelper* commands() const override { return F("|blink|stop|"); }
	public: virtual Command* resolve_cmd(const char* name) const override;
	
	protected: virtual void toggling_begin() override
	{
		toggle_val = LOW;
		digitalWrite(_pin, toggle_val);
	}
	protected: virtual void toggle() override
	{
		toggle_val = !toggle_val;
		digitalWrite(_pin, toggle_val);
	}
	protected: virtual void toggling_end() override
	{
		toggle_val = LOW;
		digitalWrite(_pin, toggle_val);
	}
};

namespace led_commands
{
	class Blink final : public Command
	{
		public: static Blink* instance() { static Blink cmd; return &cmd; }
		
		protected: Time freq;
		
		private: Blink() { }
		public: virtual ~Blink() { }
		
		public: virtual void configure() override
		{
			freq = 0;
		}
		public: virtual void accept(unsigned long argn, const char* argv) override
		{
			if (argn == 0) freq = strtoul(argv, nullptr, 0);
		}
		public: virtual bool execute() override
		{
			// [WARNING] Typeid check should be performed first
			LED* led = (LED*)component;
			Toggler::instance().toggle(led, freq);
			return true;
		}
	};

	class Stop final : public Command
	{
		public: static Stop* instance() { static Stop cmd; return &cmd; }
		
		private: Stop() { }
		public: virtual ~Stop() { }
		
		public: virtual void configure() override
		{

		}
		public: virtual void accept(unsigned long argn, const char* argv) override
		{

		}
		public: virtual bool execute() override
		{
			// [WARNING] Typeid check should be performed first
			LED* led = (LED*)component;
			Toggler::instance().stop(led);
			return true;
		}
	};
}
