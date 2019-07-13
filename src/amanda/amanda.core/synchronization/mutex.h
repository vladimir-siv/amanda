#pragma once

#include "../system.h"
#include "../thread.h"

#include <structures/specialized/vlist.h>

class mutex final
{
	private: volatile Thread* _owner = nullptr;
	private: volatile unsigned int _lock_count = 0;
	private: volatile vlist<Thread> _blocked;
	
	public: Thread* owningThread() volatile const { return const_cast<Thread*>(_owner); }
	public: unsigned int lock_count() volatile const { return _lock_count; }
	
	public: void lock() volatile
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
			_blocked.push_back(current);
			
			System::unlock();
			dispatch();
		}
	}
	public: bool try_lock() volatile
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
	public: bool unlock() volatile
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
					current = _blocked.pop_front();
					_owner = current;
					++_lock_count;

					current->setState(Thread::READY);
					System::scheduler.put(current);
				}
				else _owner = nullptr;

				hasReleased = true;
			}
		}

		System::unlock();
		return hasReleased;
	}
};
