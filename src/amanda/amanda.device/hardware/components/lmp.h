#pragma once

#include "abstraction/element.h"
#include "../../xml/api.h"

#include <thread.h>
#include <synchronization/semaphore.h>

class LMP : public DigitalElement
{
	public: explicit LMP(byte pin) : DigitalElement(pin) { }
	public: virtual const __FlashStringHelper* description() const override { return F("Lamp"); }
};

class BlinkingLMP : public LMP
{
	protected: static ThreadDelegate _delegate;
	protected: Time _freq = 0;
	protected: semaphore _sync;
	protected: Thread _blinker;
	
	public: explicit BlinkingLMP(byte pin) : LMP(pin), _sync(1), _blinker(_delegate, this) { }
	
	public: virtual const __FlashStringHelper* commands() const override { return F("|blink|stop|"); }
	public: virtual Command* resolve_cmd(const char* name) const override;
	
	public: virtual void blink(Time freq)
	{
		if (_freq == 0)
		{
			_freq = freq;
			if (_freq > 0) _sync.notify();
		}
		else _freq = freq;
	}
	public: virtual void stop()
	{
		_freq = 0;
	}
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
			BlinkingLMP* lmp = (BlinkingLMP*)component;
			lmp->blink(freq);
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
			BlinkingLMP* lmp = (BlinkingLMP*)component;
			lmp->stop();
			return true;
		}
	};
}
