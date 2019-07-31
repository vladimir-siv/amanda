#pragma once

#include "abstraction/element.h"
#include "../../xml/api.h"

class BUZZ : public AnalogElement
{
	protected: static BUZZ* _playing;
	
	public: explicit BUZZ(byte pin) : AnalogElement(pin) { }
	public: virtual const __FlashStringHelper* description() const override { return F("buzzer"); }
	
	public: virtual void write(AnalogValue value) const override { }
	
	public: virtual const __FlashStringHelper* commands() const override { return F("|play|stop|"); }
	public: virtual CommandResult execute(const Command& command) override
	{
		if (command.name == F("")) { }
		else if (command.name == F("play"))
		{
			if (!command.args[0].empty())
			{
				int freq = atoi(command.args[0].c_str());
				unsigned long dur = 0ul; // optional parameter

				if (!command.args[1].empty())
				{
					dur = atol(command.args[1].c_str());
				}

				play(freq, dur);
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
	
	public: virtual void play(int frequency, unsigned long duration = 0ul)
	{
		if (_playing != nullptr && _playing != this) _playing->stop();
		_playing = this;
		tone(_pin, frequency, duration);
	}
	public: virtual void stop()
	{
		if (_playing == this) noTone(_pin);
	}
};
