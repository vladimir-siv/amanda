#pragma once

#include <structures/list.h>

using ThreadDelegate = void(*)(...);
extern void dispatch();

extern "C" void TIMER1_COMPA_vect(void);

class Thread final
{
	friend void TIMER1_COMPA_vect(void);
	friend void dispatch();
	friend void __idle__(...);
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
	
	private: static void init();
	public: static Thread* current();
	
	private: unsigned long id;
	private: byte* stack;
	private: volatile uintptr_t sp;
	private: volatile unsigned int quantum;
	private: State state;
	private: list<Thread*> complete;
	
	private: Thread();
	public: explicit Thread(ThreadDelegate delegate, unsigned long stackSize = 128);
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
