#pragma once

#include <Arduino.h>

#include "../../hardware/components/abstraction/component.h"
#include "../../hardware/hardware_controller.h"
#include "../../hardware/events.h"
#include "../../xml/api.h"
#include "../../common/units.h"

// example of an event: "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><event repeat=\"0\"><requirements><pack><condition vid=\"4\" ctype=\"AS\"><lss>100.0</lss></condition><condition vid=\"3\" ctype=\"DS\"><equ>1</equ></condition></pack></requirements><actions><raise><write vid=\"3\" ctype=\"DE\"><state>1</state></write></raise><expire><write vid=\"3\" ctype=\"DE\"><state>0</state></write></expire></actions></event>"

class EventParser final : public xml::SAXParser
{
	private: unsigned int level;
	private: byte l2, l3, l5;
	
	private: event* evt;
	private: pack* pck;
	private: cond* cnd;
	private: comparator::cmp cmp;
	private: IComponent* comp;
	private: unit unt;
	private: struct
	{
		unsigned long vid; char ctype[3];
		void reset() { vid = 0; ctype[0] = ctype[1] = ctype[2] = 0; }
	} comp_details;
	
	public: HardwareController* controller;
	
	public: explicit EventParser(HardwareController* controller = nullptr) : controller(controller) { }
	
	protected: virtual void reset() override;
	protected: virtual void oncancel() override;
	protected: virtual void tag_opened(const char* tagname) override;
	protected: virtual void attribute_spec(const char* attrname, const char* attrvalue) override;
	protected: virtual void attribute_spec_end() override;
	protected: virtual void text_value(const char* value) override;
	protected: virtual void tag_closed(const char* tagname) override;
	
	public: event* extractEvent() const;
};
