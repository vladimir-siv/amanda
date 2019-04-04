#pragma once

#include "abstraction/element.h"
#include "../../xml/api.h"

#include <thread.h>
#include <synchronization/semaphore.h>

class LED : public DigitalElement
{
	public: LED(byte pin) : DigitalElement(pin) { }
};

class BlinkingLED : public LED
{
	protected: static ThreadDelegate _delegate;
	protected: Time _freq = 0;
	protected: semaphore _sync = semaphore(1);
	protected: Thread* _blinker = new Thread(_delegate, this);
	
	public: BlinkingLED(byte pin) : LED(pin) { }
	
	public: virtual String commands() const override { return "|blink|stop|"; }
	public: virtual CommandResult execute(const String& command) override
	{
		TiXmlDocument doc = xml::to_document(command);
		TiXmlElement* root = doc.RootElement();

		if (String(root->Value()) == "command")
		{
			String cmdname = root->Attribute("name");

			if (cmdname == "") { }
			else if (cmdname == "blink")
			{
				if (!root->NoChildren())
				{
					TiXmlElement* arg = root->FirstChildElement();
					if (arg != nullptr && String(arg->Value()) == "arg")
					{
						Time freq = String(arg->GetText()).toInt();
						blink(freq);
					}
				}
			}
			else if (cmdname == "stop")
			{
				if (root->NoChildren())
				{
					stop();
				}
			}
		}

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
