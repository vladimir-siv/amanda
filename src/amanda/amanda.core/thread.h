#pragma once

#include "def.h"

#include <structures/specialized/vlist.h>
#include <structures/tuple.h>

using ThreadDelegate = void(*)(void);
extern void dispatch();

extern "C" void TIMER1_COMPA_vect(void);

class Thread final
{
	friend void TIMER1_COMPA_vect(void);
	friend void dispatch();
	friend class System;
	friend class Scheduler;
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
	
	private: static const unsigned int _DEFAULT_QUANTUM = 50;
	
	private: static unsigned long idGen;
	private: static Thread loop;
	private: static Thread* running;
	private: static bool dispatch_idle;
	private: static Thread* idle;
	private: static vlist<Tuple<Time, Thread*>> sleeping;
	
	private: static void init();
	public: static inline void tick();
	public: static Thread* current();
	public: static void sleep(Time millis);
	
	public: template<typename T> static T current_context() { return (T)running->context; }
	
	private: unsigned long id;
	private: byte* stack;
	private: volatile uintptr_t sp;
	private: volatile unsigned int quantum;
	private: State state;
	private: vlist<Thread> complete;
	private: void* context;
	
	private: Thread();
	public: Thread(ThreadDelegate delegate, unsigned long stackSize);
	public: explicit Thread(ThreadDelegate delegate, void* context = nullptr, unsigned long stackSize = 128);
	public: Thread(const Thread& thread) = delete;
	public: Thread(Thread&& thread) = delete;
	public: ~Thread();
	
	public: Thread& operator=(const Thread& thread) = delete;
	public: Thread& operator=(Thread&& thread) = delete;
	
	private: void finish();
	private: static void finalize();
	
	public: unsigned long ID() const;
	
	public: bool isInState(State state) const;
	private: void setState(State state);
	
	public: void waitToComplete();
	public: void abort();
};
