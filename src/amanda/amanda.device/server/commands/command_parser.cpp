#include "command_parser.h"

bool CommandParser::cancel = false;
unsigned int CommandParser::level = 0;
byte CommandParser::arg = 0;
Command CommandParser::command;

void CommandParser::reset()
{
	cancel = false;
	level = 0;
	arg = 0;
	command = Command();
}

void CommandParser::tag_opened(const char* tagname)
{
	++level;

	if (level == 1)
	{
		if (strcmp(tagname, "command") != 0)
		{
			cancel = true; // invalid tag
		}
	}
	else if (level == 2)
	{
		if (arg > 4 || strcmp(tagname, "arg") != 0)
		{
			cancel = true; // too many args or invalid tag
		}
	}
	else cancel = true; // invalid nesting level
}
void CommandParser::attribute_spec(const char* attrname, const char* attrvalue)
{
	if (level == 1)
	{
		if (strcmp(attrname, "name") == 0)
		{
			command.name = attrvalue;
		}
		else cancel = true; // incorrect attribute name
	}
	else cancel = true; // attributes only allowed on root
}
void CommandParser::attribute_spec_end()
{

}
void CommandParser::text_value(const char* value)
{
	if (level == 2)
	{
		command.args[arg] = value;
	}
	else cancel = true; // incorrect text occurance (incorrect level)
}
void CommandParser::tag_closed(const char* tagname)
{
	if (level == 1)
	{
		if (tagname != nullptr && strcmp(tagname, "command") != 0)
		{
			cancel = true; // closing tag not correct
		}
	}
	else if (level == 2)
	{
		if (tagname != nullptr && strcmp(tagname, "arg") == 0)
		{
			++arg;
		}
		else cancel = true; // empty arg or closing tag not correct
	}
	else cancel = true; // incorrect level to close a tag

	--level;

	if (level == 0) cancel = true; // end parsing command
}

const Command* CommandParser::parse(const char* xml)
{
	XmlEventDispatcher events;

	events.SetTagOpenedEventHandler(&tag_opened);
	events.SetAttributeSpecEventHandler(&attribute_spec);
	events.SetAttributeSpecEventEndHandler(&attribute_spec_end);
	events.SetTextValueEventHandler(&text_value);
	events.SetTagClosedEventHandler(&tag_closed);
	
	reset();

	if (xml::parse(xml, events, &cancel)) return &command;
	
	return nullptr;
}
