#include "toggle.h"

ThreadDelegate Toggler::_delegate = []() -> void
{
	Toggler* THIS = Thread::current_context<Toggler*>();
	
	while (true)
	{
		while (THIS->lst.size() == 0) dispatch();

		Time left = THIS->try_wakeup();

		if (left == 0)
		{
			dispatch();
			continue;
		}

		if (left < 50) Thread::delay(left);
		else dispatch();
	}
};
