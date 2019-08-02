#pragma once

#include "../../process.h"
#include "../../../events/event_parser.h"
#include "../../../events/event_handler.h"
#include "../../../../common/data/flash_stream.h"

class EventCreateProcess : public Process
{
	public: static EventCreateProcess* instance() { static EventCreateProcess proc; return &proc; }
	
	protected: EventParser ev_parser;
	
	public: virtual ~EventCreateProcess() { }
	public: virtual xml::SAXParser& parser() const override
	{
		EventParser& e = (EventParser&)ev_parser;
		e.setDefaultController();
		return e;
	}
	public: virtual void invoke(bool success = true, const char* name = nullptr) override
	{
		if (success)
		{
			event* ev = ev_parser.extractEvent();

			if (ev != nullptr)
			{
				EventHandler::instance().append(ev, name);
				client->respond(data::FlashStream(F("<action>success</action>")));
				return;
			}
		}
		
		client->respond(data::FlashStream(F("<action>failure</action>")));
	}
};
