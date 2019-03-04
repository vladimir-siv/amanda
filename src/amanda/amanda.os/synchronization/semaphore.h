#pragma once

#include "../structures/vector.h"
#include "../thread.h"

class Semaphore final
{
	private: Vector<Thread*> _blocked;
	private: int _value;
	
	public: explicit Semaphore(unsigned int capacity, int _value = 0) : _blocked(capacity), _value(_value) { }
	
	public: void wait()
	{
		System::lock();

		Thread::running->setState(Thread::WAITING);
		
		System::unlock();
	}
};
