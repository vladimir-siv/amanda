#include "request.h"

// level legend:
//	l1 == 1 => task
//	l1 == 2 => process

void RequestBodyParser::reset()
{
	level = 0;
	l1 = 0;
	task = nullptr;
	proc = nullptr;
}
void RequestBodyParser::oncancel()
{
	level = 0;
	l1 = 0;
	task = nullptr;
	proc = nullptr;
}
void RequestBodyParser::tag_opened(const char* tagname)
{
	++level;

	switch (level)
	{
		case 1:
		{
			if (strcmp(tagname, "action") != 0) cancel(); // invalid tag
		} break;
		case 2:
		{
			if (l1 == 1)
			{
				if (strcmp(tagname, "arg") != 0) cancel(); // invalid tag
			}
			else cancel(); // invalid nesting level
		} break;

		default:
		{
			cancel(); // invalid nesting level
		} break;
	}
}
void RequestBodyParser::attribute_spec(const char* attrname, const char* attrvalue)
{
	switch (level)
	{
		case 1:
		{
			if (strcmp(attrname, "task") == 0)
			{
				l1 = 1;
				task = Task::resolve(attrvalue, request);
			}
			else if (strcmp(attrname, "process") == 0)
			{
				l1 = 2;
				proc = Process::resolve(attrvalue, request);
			}
			else cancel(); // invalid attribute specification
		} break;

		default:
		{
			cancel(); // invalid nesting level
		} break;
	}
}
void RequestBodyParser::attribute_spec_end()
{
	switch (level)
	{
		case 1:
		{
			if (l1 == 2)
			{
				if (proc != nullptr)
				{
					xml::SAXParser& parser = proc->parser();
					bool succ = swap(parser);
					proc->invoke(succ);
					if (!succ) cancel(); // invalid request
				}
				else cancel(); // proc not set
			}
		} break;
	}
}
void RequestBodyParser::text_value(const char* value)
{
	switch (level)
	{
		case 2:
		{
			if (l1 == 1)
			{
				if (task != nullptr)
				{
					task->accept(value);
				}
				else cancel(); // task not set
			}
			else cancel(); // invalid nesting level (should not be possible to happen)
		} break;

		default:
		{
			cancel(); // invalid nesting level
		} break;
	}
}
void RequestBodyParser::tag_closed(const char* tagname)
{
	switch (level)
	{
		case 1:
		{
			if (strcmp(tagname, "action") == 0)
			{
				if (l1 == 1)
				{
					if (task != nullptr)
					{
						l1 = 0;
						task->invoke();
						task = nullptr;
					}
					else cancel(); // task not set (should not be possible to happen)
				}
				else if (l1 == 2)
				{
					if (proc != nullptr)
					{
						l1 = 0;
						proc = nullptr;
					}
					else cancel(); // proc not set (should not be possible to happen)
				}
				else cancel(); // invalid action type (should not be possible to happen) 
			}
			else cancel(); // invalid tag
		} break;
		case 2:
		{
			if (l1 == 1)
			{
				if (strcmp(tagname, "arg") != 0) cancel(); // invalid tag
			}
			else cancel(); // invalid nesting level
		} break;

		default:
		{
			cancel(); // invalid nesting level
		} break;
	}

	if (!canceled())
	{
		--level;
		if (level == 0) finish(); // end parsing event
	}
}
