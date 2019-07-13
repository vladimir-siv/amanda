#pragma once

#include "scheduler.h"

#include "fifo.h"

class SystemScheduler final : public Scheduler
{
	friend class System;

	private: volatile Scheduler* _scheduler = FIFOScheduler::instance();
	
	public: virtual void __attribute__((noinline)) put(Thread* thread) volatile override
	{
		if (thread != nullptr)
		{
			_scheduler->put(thread);
			stop_idle();
		}
	}
	public: virtual Thread* __attribute__((noinline)) get() volatile override
	{
		Thread* thread = Thread::sleeping.take();
		if (thread == nullptr) thread = _scheduler->get();
		if (thread != nullptr) setDefaultQuantum(thread);
		return thread;
	}
};
