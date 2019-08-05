#pragma once

#include "../../task.h"
#include "../../../../common/data/flash_stream.h"

// template: "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Echo/hello\"></action>"

class HelloTask : public Task
{
	public: static HelloTask* instance() { static HelloTask task; return &task; }
	
	public: virtual ~HelloTask() { }
	
	public: virtual void invoke() override
	{
		client->respond(data::FlashStream(F("<message>Hello World.</message>")));
	}
};
