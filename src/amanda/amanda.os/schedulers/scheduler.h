#pragma once

#include "../thread.h"

class Scheduler
{
	public: virtual ~Scheduler() { }
	public: virtual void put(Thread* thread) = 0;
	public: virtual Thread* get() = 0;
	protected: void setQuantum(Thread* thread, unsigned int quantum) { thread->quantum = quantum; }
	protected: void setDefaultQuantum(Thread* thread) { setQuantum(thread, Thread::_DEFAULT_QUANTUM); }
};
