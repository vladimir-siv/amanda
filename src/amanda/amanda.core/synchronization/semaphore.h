#pragma once

#include "../system.h"
#include "../thread.h"
#include "mutex.h"
#include "lock.h"
#include "condition.h"

#include <structures/vector.h>
#include <exceptions.h>

class semaphore final
{
	private: mutex _mtx;
	private: condition _cnd;
	private: int _value;
	
	public: explicit semaphore(int value = 0) : _value(value)
	{
		if (value < 0) Exceptions::Throw<ArgumentException>();
	}
	
	public: int value() const
	{
		return _value;
	}
	public: void set_value(int value)
	{
		lock lck(_mtx);
		if (_value >= 0 && value >= 0) _value = value;
	}
	
	public: void wait()
	{
		lock lck(_mtx);

		--_value;

		if (_value < 0) _cnd.wait(&lck);
	}
	public: void notify()
	{
		lock lck(_mtx);

		if (_value < 0) _cnd.notify();
		++_value;
	}
	public: void notify_all()
	{
		lock lck(_mtx);

		if (_value < 0)
		{
			_cnd.notify_all();
			_value = 0;
		}
	}
};
