#pragma once

#include <Arduino.h>
#include <networking/xml/xml_parser.h>

namespace xml
{
	bool parse(const char* xml, const XmlEventDispatcher& events, bool* cancelToken = nullptr);
}
