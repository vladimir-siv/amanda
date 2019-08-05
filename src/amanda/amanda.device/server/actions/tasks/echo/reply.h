#pragma once

#include "../../task.h"
#include "../../../../common/data/flash_stream.h"
#include "../../../../lib/string64.h"

// template: "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Echo/reply\"><arg>{message}</arg></action>"

class ReplyTask : public Task
{
	public: static ReplyTask* instance() { static ReplyTask task; return &task; }
	
	protected: String64 message;
	
	public: virtual ~ReplyTask() { }
	public: virtual void accept(const char* arg) override
	{
		message = arg;
	}
	public: virtual void invoke() override
	{
		client->respond(data::FlashStream(F("<message>")));
		client->respond((const uint8_t*)message.c_str(), message.length());
		client->respond(data::FlashStream(F("</message>")));
	}
};
