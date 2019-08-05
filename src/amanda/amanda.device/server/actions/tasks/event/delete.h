#pragma once

#include <Arduino.h>

#include "../../task.h"

#include "../../../events/event_handler.h"
#include "../../../../common/data/flash_stream.h"

// template: "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Event/delete\"><arg>{id}</arg></action>"

class EventDeleteTask : public Task
{
	public: static EventDeleteTask* instance() { static EventDeleteTask task; return &task; }
	
	protected: enum Operation { NONE = 0, READ = 1, WRITE = 2 };
	
	protected: unsigned long id;
	
	public: virtual ~EventDeleteTask() { }
	
	public: virtual void configure(const char* name = nullptr) override
	{
		id = 0;
	}
	public: virtual void accept(const char* arg) override
	{
		id = strtoul(arg, nullptr, 0);
	}
	public: virtual void invoke() override
	{
		if (id != 0)
		{
			if (EventHandler::instance().remove(id))
			{
				client->respond(data::FlashStream(F("<action>success</action>")));
				return;
			}
		}

		client->respond(data::FlashStream(F("<action>failure</action>")));
	}
};
