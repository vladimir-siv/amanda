#pragma once

#include "../system.h"
#include "../thread.h"
#include "lock.h"

#include <structures/specialized/vlist.h>

class condition final
{
	private: volatile vlist<Thread> _blocked;
	
	public: void wait(lock* lck = nullptr) volatile
	{
		System::lock();

		Thread* current = Thread::current();
		current->setState(Thread::WAITING);
		_blocked.push_back(current);
		if (lck) lck->mtx.unlock();

		System::unlock();
		dispatch();
	}
	public: void notify() volatile
	{
		System::lock();

		if (_blocked.size() > 0)
		{
			Thread* thread = _blocked.pop_front();
			thread->setState(Thread::READY);
			System::scheduler.put(thread);
		}

		System::unlock();
	}
	public: void notify_all() volatile
	{
		System::lock();

		while (_blocked.size() > 0)
		{
			Thread* thread = _blocked.pop_front();
			thread->setState(Thread::READY);
			System::scheduler.put(thread);
		}

		System::unlock();
	}
};
