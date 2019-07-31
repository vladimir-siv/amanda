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
	public: virtual CommandResult execute(const Command& command) override
	{
		if (command.name == F("")) { }
		else if (command.name == F("blink"))
		{
			if (!command.args[0].empty() && command.args[1].empty())
			{
				Time freq = atol(command.args[0].c_str());
				blink(freq);
			}
		}
		else if (command.name == F("stop"))
		{
			if (command.args[0].empty())
			{
				stop();
			}
		}
		else { }

		return CommandResult::null();
	};
	
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
