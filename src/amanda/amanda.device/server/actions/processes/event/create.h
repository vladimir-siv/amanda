#pragma once

#include "../../process.h"
#include "../../../events/event_parser.h"
#include "../../../events/event_handler.h"
#include "../../../../common/data/flash_stream.h"

// template: "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action process=\"Event/create:{event-name}\">{inner-event-xml}</action>"

class EventCreateProcess : public Process
{
	public: static EventCreateProcess* instance() { static EventCreateProcess proc; return &proc; }
	
	protected: EventParser ev_parser;
	
	public: virtual ~EventCreateProcess() { }
	public: virtual xml::SAXParser& parser() const override
	{
		// [NOTE] Setting controller has to be done here, it cannot be done in the constructor.
		// The reason is because the client object is set after each resolve, and not upon the
		// object creation.

		EventParser& e = (EventParser&)ev_parser;
		e.controller = client->get_handling_server()->get_bound_controller();
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
