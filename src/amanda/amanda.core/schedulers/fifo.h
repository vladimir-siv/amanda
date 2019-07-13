#pragma once

#include <exceptions.h>
#include <structures/specialized/vlist.h>
#include "scheduler.h"

class FIFOScheduler : public Scheduler
{
	friend class System;
	friend class SystemScheduler;

	public: FIFOScheduler(const FIFOScheduler& scheduler) = delete;
	public: FIFOScheduler(FIFOScheduler&& scheduler) = delete;
	public: FIFOScheduler& operator=(const FIFOScheduler& scheduler) = delete;
	public: FIFOScheduler& operator=(FIFOScheduler&& scheduler) = delete;
	
	protected: static FIFOScheduler* instance() { static FIFOScheduler _scheduler; return &_scheduler; }
	
	protected: volatile vlist<Thread> threads;
	
	protected: FIFOScheduler() { }
	
	public: unsigned int size() volatile const { return threads.size(); }
	public: Thread* next() volatile const { return threads.peek_front(); }
	public: Thread* last() volatile const { return threads.peek_back(); }
	
	public: virtual void put(Thread* thread) volatile override
	{
		if (thread != nullptr) threads.push_back(thread);
		return;
	}
	public: virtual Thread* get() volatile override
	{
		if (size() == 0) return nullptr;
		return threads.pop_front();
	}
};
