#pragma once

#include "abstraction/element.h"
#include "functions/toggle.h"

class LMP : public DigitalElement, public Toggleable
{
	protected: DigitalState toggle_val = LOW;
	
	public: explicit LMP(byte pin) : DigitalElement(pin) { }
	public: virtual const __FlashStringHelper* description() const override { return F("Lamp"); }
	
	public: virtual const __FlashStringHelper* commands() const override { return F("|blink|stop|"); }
	public: virtual Command* resolve_cmd(const char* name) const override;
	
	protected: virtual void _toggling_begin() override { write(LOW); }
	protected: virtual void _toggle() override { toggle(); }
	protected: virtual void _toggling_end() override { write(LOW); }
};

namespace lmp_commands
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
			LMP* lmp = (LMP*)component;
			Toggler::instance().toggle(lmp, freq);
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
			LMP* lmp = (LMP*)component;
			Toggler::instance().stop(lmp);
			return true;
		}
	};
}
