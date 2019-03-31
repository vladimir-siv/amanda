#pragma once

#include "../system.h"
#include "../thread.h"
#include "lock.h"

#include <structures/queue.h>

class condition final
{
	private: queue<Thread*> _blocked;
	
	public: condition(unsigned int capacity = 10) : _blocked(queue<Thread*>(capacity)) { }
	
	public: unsigned int capacity() const { return _blocked.capacity(); }
	
	public: void wait(lock* lck = nullptr)
	{
		System::lock();

		Thread* current = Thread::current();
		current->setState(Thread::WAITING);
		_blocked.enqueue(current);
		if (lck) lck->mtx.unlock();

		System::unlock();
		dispatch();
	}
	public: void notify()
	{
		System::lock();

		if (_blocked.size() > 0)
		{
			Thread* thread = _blocked.dequeue();
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
			Thread* thread = _blocked.dequeue();
			thread->setState(Thread::READY);
			System::scheduler->put(thread);
		}

		System::unlock();
	}
};
