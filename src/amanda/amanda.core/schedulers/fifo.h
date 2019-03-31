#pragma once

#include <exceptions.h>
#include "scheduler.h"

class FIFOScheduler : public Scheduler
{
	friend class System;

	protected: Thread** threads;
	protected: unsigned int capacity, size;
	protected: unsigned int head, tail;
	
	protected: explicit FIFOScheduler(unsigned int capacity) : threads(new Thread*[capacity]), capacity(capacity), size(0), head(0), tail(0) { }
	public: virtual ~FIFOScheduler() override { delete[] threads; threads = nullptr; capacity = size = 0; head = tail = 0; }
	
	public: FIFOScheduler(const FIFOScheduler& scheduler) = delete;
	public: FIFOScheduler(FIFOScheduler&& scheduler) = delete;
	public: FIFOScheduler& operator=(const FIFOScheduler& scheduler) = delete;
	public: FIFOScheduler& operator=(FIFOScheduler&& scheduler) = delete;
	
	public: virtual void put(Thread* thread) override
	{
		if (size == capacity) { Exceptions::Throw<CollectionFullException>(); return; }
		threads[tail] = thread;
		if (++tail == capacity) tail = 0;
		++size;
		stop_idle();
		return;
	}
	public: virtual Thread* get() override
	{
		if (size == 0) return nullptr;
		Thread* thread = threads[head];
		if (++head == capacity) head = 0;
		--size;
		setDefaultQuantum(thread);
		return thread;
	}
};
