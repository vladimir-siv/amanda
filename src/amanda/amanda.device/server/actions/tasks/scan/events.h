#pragma once

#include <Arduino.h>

#include "../../task.h"

#include "../../../events/event_handler.h"

// template: "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Scan/events\"></action>"

class ScanEventsTask : public Task
{
	public: static ScanEventsTask* instance() { static ScanEventsTask task; return &task; }
	
	public: virtual ~ScanEventsTask() { }
	
	public: virtual void invoke() override
	{
		EventHandler::instance().output_events(*client);
	}
};
