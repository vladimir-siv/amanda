#include "api.h"

XmlParser parser;

namespace xml
{
	bool parse(const char* xml, const XmlEventDispatcher& events, bool* cancelToken)
	{
		if (cancelToken != nullptr && *cancelToken) return false;

		parser.reset();

		parser.events = events;

		while (*xml != 0)
		{
			if (!parser.nextchar(*(xml++))) return false;
			if (cancelToken != nullptr && *cancelToken) return false;
		}

		return true;
	}
}
