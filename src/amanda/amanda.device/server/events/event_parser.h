#pragma once

#include "../../hardware/components/abstraction/component.h"
#include "../../xml/api.h"
#include "../../lib/string64.h"
#include "../../hardware/events.h"

// example of an event: "<event repeat=\"0\"><requirements><pack><condition vid=\"4\" ctype=\"AS\"><lss>100.0</lss></condition><condition vid=\"3\" ctype=\"DS\"><equ>1</equ></condition></pack></requirements><actions><raise><write vid=\"3\" ctype=\"DE\"><state>1</state></write></raise><expire><write vid=\"3\" ctype=\"DE\"><state>0</state></write></expire></actions></event>"

class EventParser final : public xml::SAXParser
{	
	protected: virtual void reset() override;
	protected: virtual void tag_opened(const char* tagname) override;
	protected: virtual void attribute_spec(const char* attrname, const char* attrvalue) override;
	protected: virtual void attribute_spec_end() override;
	protected: virtual void text_value(const char* value) override;
	protected: virtual void tag_closed(const char* tagname) override;
};
