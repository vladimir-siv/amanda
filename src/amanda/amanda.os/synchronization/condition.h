#pragma once

#include <structures/list.h>
#include "../system.h"
#include "../thread.h"

class condition final
{
	private: list<Thread*> _blocked;
	
	public: void wait()
	{
		System::lock();

		Thread* current = Thread::current();
		current->setState(Thread::WAITING);
		_blocked.push_back(current);

		System::unlock();
		dispatch();
	}
	public: void notify()
	{
		System::lock();

		if (_blocked.size() > 0)
		{
			Thread* thread = _blocked.pop_front();
			thread->setState(Thread::READY);
			System::scheduler->put(thread);
		}

		System::unlock();
	}
	public: void notify_all()
	{
		System::lock();

		while (_blocked.size() > 0)
		{
			Thread* thread = _blocked.pop_front();
			thread->setState(Thread::READY);
			System::scheduler->put(thread);
		}

		System::unlock();
	}
};
