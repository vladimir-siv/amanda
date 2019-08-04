#include "request.h"
#include "../lib/string64.h"

// level legend:
//	l1 == 1 => task
//	l1 == 2 => process

void RequestBodyParser::reset()
{
	level = 0;
	l1 = 0;
	task = nullptr;
	proc = nullptr;
	action_name = nullptr; // [WARNING] Deallocate!
}
void RequestBodyParser::oncancel()
{
	level = 0;
	l1 = 0;
	task = nullptr;
	proc = nullptr;
	action_name = nullptr; // [WARNING] Deallocate!
}
void RequestBodyParser::tag_opened(const char* tagname)
{
	++level;

	switch (level)
	{
		case 1:
		{
			if (strcmp_P(tagname, PSTR("action")) != 0) cancel(); // invalid tag
		} break;
		case 2:
		{
			if (l1 == 1)
			{
				if (strcmp_P(tagname, PSTR("arg")) != 0) cancel(); // invalid tag
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
	// [WARNING] THIS METHOD SHOULD RETURN A COPY OF THE SUBSTRING, INSTEAD
	// IT'S CHANGING THE GIVEN ONE & RETURNING IT AS AN OPTIMIZATION WHICH
	// ENTIRELY DEPENDS ON XML_PARSER ACCIDENTLY STILL HAVING THE CORRECT VALUE
	// OF THE LAST ATTRIBUTE SPECIFICATION STILL IN ATTRIBUTE SPEC END EVENT.
	// THUS, THERE IS NO NEED FOR ANY NEW ALLOCATION/COPY/DEALLOCATION BUT WILL
	// WORK ONLY IF THE ABOVE IS CORRECT. SHOULD YOU EVER CHANGE THAT, THIS
	// PIECE OF CODE WILL NOT WORK!
	static auto split = [](const char* value) -> const char*
	{
		while (*value != 0 && *value != ':') value++;
		if (*value == 0) return nullptr; // colon not found

		*((char*)value) = 0;
		return value + 1;
	};

	switch (level)
	{
		case 1:
		{
			if (strcmp_P(attrname, PSTR("task")) == 0)
			{
				l1 = 1;
				action_name = split(attrvalue);
				task = Task::resolve(attrvalue, request);
			}
			else if (strcmp_P(attrname, PSTR("process")) == 0)
			{
				l1 = 2;
				action_name = split(attrvalue);
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
			switch (l1)
			{
				case 1:
				{
					if (task != nullptr)
					{
						task->configure(action_name);

						if (action_name != nullptr)
						{
							((char*)action_name)[-1] = ':';
							action_name = nullptr; // [WARNING] Deallocate!
						}
					}
					else cancel(); // task not set
				} break;
				case 2:
				{
					if (proc != nullptr)
					{
						xml::SAXParser& parser = proc->parser();
						String64 name = action_name;

						if (action_name != nullptr)
						{
							((char*)action_name)[-1] = ':';
							action_name = nullptr; // [WARNING] Deallocate!
						}

						bool succ = swap(parser);
						proc->invoke(succ, name.c_str());
						if (!succ) cancel(); // invalid request
					}
					else cancel(); // proc not set
				} break;

				default:
				{
					cancel(); // invalid state (should not be possible to happen)
				} break;
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
			if (strcmp_P(tagname, PSTR("action")) == 0)
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
				if (strcmp_P(tagname, PSTR("arg")) != 0) cancel(); // invalid tag
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
