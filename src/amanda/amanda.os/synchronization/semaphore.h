#pragma once

#include <structures/vector.h>
#include <exceptions.h>
#include "../system.h"
#include "../thread.h"

class semaphore final
{
	private: vector<Thread*> _blocked;
	private: int _value;
	
	public: explicit semaphore(unsigned int capacity, int _value = 0) : _blocked(capacity), _value(_value)
	{
		if (_value < 0) Exceptions::Throw<ArgumentException>();
	}
	
	public: int value() const
	{
		return _value;
	}
	public: void set_value(int value)
	{
		if (_value >= 0 && value >= 0) _value = value;
	}
	
	public: void wait()
	{
		System::lock();

		if (-_value >= _blocked.capacity())
		{
			Exceptions::Throw<CollectionFullException>();
			System::unlock();
			return;
		}

		--_value;

		if (_value < 0)
		{
			Thread* current = Thread::current();
			current->setState(Thread::WAITING);
			_blocked.push(current);

			System::unlock();
			dispatch();
			return;
		}

		System::unlock();
	}
	public: void notify()
	{
		System::lock();

		if (_value < 0)
		{
			Thread* thread = _blocked.pop();
			thread->setState(Thread::READY);
			System::scheduler->put(thread);
		}

		++_value;

		System::unlock();
	}
	public: void notify_all()
	{
		System::lock();

		while (_blocked.size() > 0)
		{
			Thread* thread = _blocked.pop();
			thread->setState(Thread::READY);
			System::scheduler->put(thread);
		}

		_value = 0;

		System::unlock();
	}
};
