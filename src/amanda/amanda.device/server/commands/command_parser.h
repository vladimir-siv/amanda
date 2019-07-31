#pragma once

#include <Arduino.h>

#include "../../hardware/components/abstraction/component.h"
#include "../../xml/api.h"
#include "../../lib/string64.h"

// example of a command string: "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><command name=\"blink\"><arg>1000</arg></command>"

class CommandParser final : public xml::SAXParser
{
	public: static CommandParser& instance()
	{
		static CommandParser parser;
		return parser;
	}
	
	private: CommandParser() { }
	
	private: unsigned int level;
	private: byte arg;
	private: Command command;
	
	protected: virtual void reset() override;
	protected: virtual void tag_opened(const char* tagname) override;
	protected: virtual void attribute_spec(const char* attrname, const char* attrvalue) override;
	protected: virtual void attribute_spec_end() override;
	protected: virtual void text_value(const char* value) override;
	protected: virtual void tag_closed(const char* tagname) override;
	
	public: Command& extractCommand();
};
