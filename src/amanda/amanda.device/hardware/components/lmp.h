#pragma once

#include "abstraction/element.h"
#include "../../xml/api.h"

#include <thread.h>
#include <synchronization/semaphore.h>

class LMP : public DigitalElement
{
	public: explicit LMP(byte pin) : DigitalElement(pin) { }
	public: virtual const char* description() const override { return "Lamp"; }
};

class BlinkingLMP : public LMP
{
	protected: static ThreadDelegate _delegate;
	protected: Time _freq = 0;
	protected: semaphore _sync = semaphore(1);
	protected: Thread _blinker;
	
	public: explicit BlinkingLMP(byte pin) : LMP(pin), _blinker(_delegate, this) { }
	
	public: virtual const char* commands() const override { return "|blink|stop|"; }
	public: virtual CommandResult execute(const Command& command) override
	{
		if (command.name == "") { }
		else if (command.name == "blink")
		{
			if (!command.args[0].empty() && command.args[1].empty())
			{
				Time freq = atol(command.args[0].c_str());
				blink(freq);
			}
		}
		else if (command.name == "stop")
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
