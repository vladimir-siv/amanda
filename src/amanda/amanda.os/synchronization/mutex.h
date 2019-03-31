#pragma once

#include "../system.h"
#include "../thread.h"

#include <structures/queue.h>

class mutex final
{
	private: Thread* _owner = nullptr;
	private: unsigned int _lock_count = 0;
	private: queue<Thread*> _blocked;
	
	public: mutex(unsigned int capacity = 10) : _blocked(queue<Thread*>(capacity)) { }
	
	public: Thread* owningThread() const { return _owner; }
	public: unsigned int lock_count() const { return _lock_count; }
	public: unsigned int capacity() const { return _blocked.capacity(); }
	
	public: void lock()
	{
		System::lock();

		Thread* current = Thread::current();

		if (_owner == nullptr || _owner == current)
		{
			_owner = current;
			++_lock_count;

			System::unlock();
		}
		else
		{
			current->setState(Thread::WAITING);
			_blocked.enqueue(current);

			System::unlock();
			dispatch();
		}
	}
	public: bool try_lock()
	{
		System::lock();

		Thread* current = Thread::current();

		if (_owner == nullptr || _owner == current)
		{
			_owner = current;
			++_lock_count;

			System::unlock();
			return true;
		}
		else
		{
			System::unlock();
			return false;
		}
	}
	public: bool unlock()
	{
		System::lock();

		Thread* current = Thread::current();
		bool hasReleased = false;

		if (_owner == current)
		{
			--_lock_count;

			if (_lock_count == 0)
			{
				if (_blocked.size() > 0)
				{
					current = _blocked.dequeue();
					_owner = current;
					++_lock_count;

					current->setState(Thread::READY);
					System::scheduler->put(current);
				}
				else _owner = nullptr;

				hasReleased = true;
			}
		}

		System::unlock();
		return hasReleased;
	}
};
