#pragma once

#include <Arduino.h>

#include "../../task.h"

#include "../../../../common/data/flash_stream.h"

// template: "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"Cmd/exec\"><arg>{vid}:{ctype}</arg><arg>{cmd_name}</arg><arg>{args...}</arg>...</action>"

class CommandExecuteTask : public Task
{
	public: static CommandExecuteTask* instance() { static CommandExecuteTask task; return &task; }
	
	protected: unsigned long arg_cnt;
	protected: IComponent* component;
	protected: Command* cmd;
	
	public: virtual ~CommandExecuteTask() { }
	
	public: virtual void configure(const char* name = nullptr) override
	{
		arg_cnt = 0;
		component = nullptr;
		cmd = nullptr;
	}
	public: virtual void accept(const char* arg) override
	{
		switch (arg_cnt)
		{
			case 0:
			{
				char* cid = (char*)arg;

				unsigned int colon = 0;
				for (char* i = cid; *i != 0 && *i != ':'; ++i) ++colon;
				
				if (cid[colon] == ':')
				{
					HardwareController* controller = client->get_handling_server()->get_bound_controller();

					if (controller != nullptr)
					{
						cid[colon] = 0;

						component = controller->find(strtoul(cid, nullptr, 0), cid + colon + 1);
						
						cid[colon] = ':';
					}
				}
			} break;
			case 1:
			{
				if (component != nullptr)
				{
					cmd = component->resolve_cmd(arg);

					if (cmd != nullptr && cmd->get_component() == component)
					{
						cmd->configure();
					}
				}
			} break;

			default:
			{
				if (component != nullptr && cmd != nullptr && cmd->get_component() == component)
				{
					cmd->accept(arg_cnt - 2, arg);
				}
			} break;
		}

		++arg_cnt;
	}
	public: virtual void invoke() override
	{
		if (component != nullptr && cmd != nullptr && cmd->get_component() == component)
		{
			bool succ = cmd->execute();
			bool responded = cmd->respond(*client, succ);

			if (responded) return;

			if (succ)
			{
				client->respond(data::FlashStream(F("<action>success</action>")));
				return;
			}
		}

		client->respond(data::FlashStream(F("<action>failure</action>")));
	}
};
