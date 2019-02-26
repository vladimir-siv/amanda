#pragma once

#include "schedulers/scheduler.h"

extern "C" void TIMER1_COMPA_vect(void);

using byte = unsigned char;
using pinstate = int;

class System final
{
	friend void TIMER1_COMPA_vect(void);

	private: static volatile unsigned long _lock;
	private: static volatile unsigned long _millis;
	
	private: static Scheduler* _scheduler;
	
	public: static void init();
	
	public: static unsigned long millis();
	public: static Scheduler* scheduler();
	public: static void lock();
	public: static void unlock();
	public: static bool locked();
};
