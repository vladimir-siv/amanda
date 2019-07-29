#pragma once

#include "../../task.h"
#include "../../../../common/data/flash_stream.h"

class HelloTask : public Task
{
	public: static HelloTask* instance() { static HelloTask task; return &task; }
	
	public: virtual ~HelloTask() { }
	public: virtual void accept(const char* arg) override { }
	public: virtual void invoke() override
	{
		client->respond(data::FlashStream(F("<message>Hello World.</message>")));
	}
};
