#pragma once

#include "schedulers/scheduler.h"

extern "C" void TIMER1_COMPA_vect(void);

using byte = unsigned char;
using pinstate = int;

class System final
{
	friend class Thread;
	friend class mutex;
	friend class condition;
	friend class semaphore;
	friend void TIMER1_COMPA_vect(void);
	friend void dispatch();

	private: static volatile unsigned long _lock;
	private: static volatile unsigned long _millis;
	
	private: static Scheduler* scheduler;
	
	public: static void init();
	
	public: static unsigned long millis();
	public: static void lock();
	public: static void unlock();
	public: static bool locked();
};
