#pragma once

#include "def.h"

#include <structures/specialized/vlist.h>
#include <structures/tuple.h>

#include "extensions/structures/sleep.h"

#define _FIXED_STACK_SIZE_

// constraints:
// - NodeAllocator
// - sleep

using ThreadDelegate = void (*)(void);
extern void dispatch();

extern "C" void TIMER1_COMPA_vect(void);

class Thread final
{
	friend void TIMER1_COMPA_vect(void);
	friend void dispatch();
	friend class System;
	friend class Scheduler;
	friend class SystemScheduler;
	friend class mutex;
	friend class condition;
	friend class semaphore;

	public: enum State
	{
		READY = 1,
		RUNNING = 2,
		WAITING = 4,
		ABORTING = 8,
		FINISHED = 16,
		LOOP = 32
	};
	
#ifdef _FIXED_STACK_SIZE_
	private: static const unsigned int _STACK_SIZE = 128;
#endif
	private: static const unsigned int _DEFAULT_QUANTUM = 50;
	
	private: static volatile unsigned long idGen;
	private: static volatile Thread loop;
	private: static volatile Thread* running;
	private: static volatile bool dispatch_idle;
	private: static volatile Thread idle;
	private: static volatile sleep sleeping;
	
	private: static void __idle__(void);
	public: static void tick();
	public: static Thread* current();
	public: static void sleep(Time millis);
	public: static void delay(Time millis);
	
	public: template <typename T> static T current_context() { return (T)(running->context); }
	
	private: unsigned long id;
#ifdef _FIXED_STACK_SIZE_
	private: volatile byte stack[_STACK_SIZE];
#else
	private: volatile byte* stack = nullptr;
#endif
	private: volatile uintptr_t sp;
	private: volatile unsigned int quantum;
	private: volatile State state;
	private: volatile vlist<Thread> complete;
	private: volatile void* context;
	
	private: Thread();
	public: Thread(ThreadDelegate delegate, unsigned long stackSize);
	public: explicit Thread(ThreadDelegate delegate, void* context = nullptr, unsigned long stackSize = 128);
	public: Thread(const Thread& thread) = delete;
	public: Thread(Thread&& thread) = delete;
	public: ~Thread();
	
	public: Thread& operator=(const Thread& thread) = delete;
	public: Thread& operator=(Thread&& thread) = delete;
	
	private: void finish() volatile;
	private: static void finalize();
	
	public: unsigned long ID() volatile const;
	
	public: bool isInState(State state) volatile const;
	private: void setState(State state) volatile;
	
	public: void waitToComplete() volatile;
	public: void abort() volatile;
};
