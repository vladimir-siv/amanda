#pragma once

#include "../../hardware/components/abstraction/component.h"
#include "../../xml/api.h"
#include "../../lib/string64.h"

// example of a command string: "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><command name=\"blink\"><arg>1000</arg></command>"

class CommandParser final
{
	private: virtual ~CommandParser() = 0;
	
	private: static bool cancel;
	private: static unsigned int level;
	private: static byte arg;
	private: static Command command;
	
	private: static void reset();
	
	private: static void tag_opened(const char* tagname);
	private: static void attribute_spec(const char* attrname, const char* attrvalue);
	private: static void attribute_spec_end();
	private: static void text_value(const char* value);
	private: static void tag_closed(const char* tagname);
	
	public: static const Command* parse(const char* xml);
};
