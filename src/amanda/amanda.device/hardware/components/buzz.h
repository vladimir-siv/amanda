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
	public: virtual Command* resolve_cmd(const char* name) const override;
	
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

namespace buzz_commands
{
	class Play final : public Command
	{
		public: static Play* instance() { static Play cmd; return &cmd; }
		
		int freq;
		unsigned long dur = 0ul; // optional parameter
		
		private: Play() { }
		public: virtual ~Play() { }
		
		public: virtual void configure() override
		{
			freq = 0;
			dur = 0ul;
		}
		public: virtual void accept(unsigned long argn, const char* argv) override
		{
			if (argn == 0) freq = (int)strtol(argv, nullptr, 0);
			if (argn == 1) dur = strtoul(argv, nullptr, 0);
		}
		public: virtual bool execute() override
		{
			// [WARNING] Typeid check should be performed first
			BUZZ* buzz = (BUZZ*)component;
			buzz->play(freq, dur);
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
			BUZZ* buzz = (BUZZ*)component;
			buzz->stop();
			return true;
		}
	};
}
