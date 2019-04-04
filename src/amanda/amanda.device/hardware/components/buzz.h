#pragma once

#include "abstraction/element.h"
#include "../../xml/api.h"

class BUZZ : AnalogElement
{
	protected: static BUZZ* _playing;
	
	public: BUZZ(byte pin) : AnalogElement(pin) { }
	
	public: virtual void write(AnalogValue value) const override { }
	
	public: virtual String commands() const override { return "|play|stop|"; }
	public: virtual CommandResult execute(const String& command) override
	{
		TiXmlDocument doc = xml::to_document(command);
		TiXmlElement* root = doc.RootElement();

		if (String(root->Value()) == "command")
		{
			String cmdname = root->Attribute("name");

			if (cmdname == "") { }
			else if (cmdname == "play")
			{
				if (!root->NoChildren())
				{
					TiXmlElement* arg1 = root->FirstChildElement();
					TiXmlElement* arg2 = arg1 != nullptr ? arg1->NextSiblingElement() : nullptr;

					if (arg1 != nullptr && String(arg1->Value()) == "arg")
					{
						int freq = String(arg1->GetText()).toInt();
						unsigned long dur = 0ul; // optional parameter

						if (arg2 != nullptr && String(arg2->Value()) == "arg")
						{
							dur = String(arg2->GetText()).toInt();
						}

						play(freq, dur);
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
