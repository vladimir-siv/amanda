#pragma once

#include "scheduler.h"

#include "fifo.h"

class SystemScheduler final : public Scheduler
{
	friend class System;

	private: Scheduler* _scheduler = FIFOScheduler::instance();
	
	public: virtual void put(Thread* thread) override
	{
		_scheduler->put(thread);
		stop_idle();
	}
	public: virtual Thread* get() override
	{
		Thread* thread = Thread::sleeping.take();
		if (thread == nullptr) thread = _scheduler->get();
		if (thread != nullptr) setDefaultQuantum(thread);
		return thread;
	}
};
