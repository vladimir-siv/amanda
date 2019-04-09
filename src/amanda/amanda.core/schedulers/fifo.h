#pragma once

#include <exceptions.h>
#include <structures/specialized/vlist.h>
#include "scheduler.h"

class FIFOScheduler : public Scheduler
{
	friend class System;

	public: FIFOScheduler(const FIFOScheduler& scheduler) = delete;
	public: FIFOScheduler(FIFOScheduler&& scheduler) = delete;
	public: FIFOScheduler& operator=(const FIFOScheduler& scheduler) = delete;
	public: FIFOScheduler& operator=(FIFOScheduler&& scheduler) = delete;
	
	protected: static FIFOScheduler* instance() { static FIFOScheduler _scheduler; return &_scheduler; }
	
	protected: vlist<Thread> threads;
	
	protected: FIFOScheduler() { }
	
	public: unsigned int size() const { return threads.size(); }
	public: Thread* next() const { return threads.peek_front(); }
	public: Thread* last() const { return threads.peek_back(); }
	
	public: virtual void put(Thread* thread) override
	{
		threads.push_back(thread);
		stop_idle();
		return;
	}
	public: virtual Thread* get() override
	{
		if (size() == 0) return nullptr;
		Thread* thread = threads.pop_front();
		setDefaultQuantum(thread);
		return thread;
	}
};
