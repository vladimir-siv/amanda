#pragma once

#include <Arduino.h>

#include "../../task.h"

#include "../../../events/event_handler.h"

#include "../../../../common/data/flash_stream.h"

// template: "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Scan/hardware\"></action>"

class ScanHardwareTask : public Task
{
	public: static ScanHardwareTask* instance() { static ScanHardwareTask task; return &task; }
	
	public: virtual ~ScanHardwareTask() { }
	
	public: virtual void configure(const char* name = nullptr) override
	{
		
	}
	public: virtual void accept(const char* arg) override
	{
		
	}
	public: virtual void invoke() override
	{
		HardwareController* controller = client->get_handling_server()->get_bound_controller();

		if (controller != nullptr)
		{
			controller->scan(client);
			return;
		}

		client->respond(data::FlashStream(F("<action>failure</action>")));
	}
};
