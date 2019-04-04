#pragma once

#include <Arduino.h>
#include "tinyxml.h"

namespace xml
{
	TiXmlDocument to_document(String&& str);
	TiXmlDocument to_document(const String& str);
	String to_string(const TiXmlDocument& doc);
}
