#include "command_parser.h"

void CommandParser::reset()
{
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
			cancel(); // invalid tag
		}
	}
	else if (level == 2)
	{
		if (arg > 4 || strcmp(tagname, "arg") != 0)
		{
			cancel(); // too many args or invalid tag
		}
	}
	else cancel(); // invalid nesting level
}
void CommandParser::attribute_spec(const char* attrname, const char* attrvalue)
{
	if (level == 1)
	{
		if (strcmp(attrname, "name") == 0)
		{
			command.name = attrvalue;
		}
		else cancel(); // incorrect attribute name
	}
	else cancel(); // attributes only allowed on root
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
	else cancel(); // incorrect text occurance (incorrect level)
}
void CommandParser::tag_closed(const char* tagname)
{
	if (level == 1)
	{
		if (tagname != nullptr && strcmp(tagname, "command") != 0)
		{
			cancel(); // closing tag not correct
		}
	}
	else if (level == 2)
	{
		if (tagname != nullptr && strcmp(tagname, "arg") == 0)
		{
			++arg;
		}
		else cancel(); // empty arg or closing tag not correct
	}
	else cancel(); // incorrect level to close a tag

	if (!canceled())
	{
		--level;
		if (level == 0) finish(); // end parsing command
	}
}
Command& CommandParser::extractCommand()
{
	return command;
}
