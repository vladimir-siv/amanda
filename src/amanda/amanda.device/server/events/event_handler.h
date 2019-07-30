#pragma once

#include <structures/specialized/vlist.h>

#include "../../hardware/components/abstraction/component.h"
#include "../../hardware/hardware_controller.h"
#include "../../hardware/events.h"

class EventHandler final
{
	public: static EventHandler& instance() { static EventHandler handler; return handler; }
	
	private: vlist<event> events;
	
	private: EventHandler() { }
	
	public: void append(const event* e)
	{
		events.push_back(e);
	}
	public: void check_events() const
	{
		for (auto i = events.cbegin(); i != events.cend(); ++i)
		{
			i->check();
		}
	}
};
