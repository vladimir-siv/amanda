#include <Arduino.h>
#include "system.h"
#include "schedulers/fifo.h"
#include "thread.h"

volatile unsigned long long System::_lock = 0;
volatile unsigned long long System::_millis = 0;
Scheduler* System::scheduler = new FIFOScheduler(10); // should be 4: 2 threads, loop & idle (or 3)

void System::init()
{
	cli();

	TCCR1A = 0;
	TCCR1B = _BV(WGM12);

	TCCR1B |= _BV(CS10);	// Prescaler /1
	OCR1A = 15999;			// 1ms period @ Prescaler /1

	TCNT1 = 0;
	TIMSK1 |= _BV(OCIE1A);

	Thread::init();

	sei();
}

Time System::millis()
{
	return _millis;
}

void System::lock()
{
	if (_lock < ~0ul) ++_lock;
}

void System::unlock()
{
	if (_lock > 0ul) --_lock;
}

bool System::locked()
{
	return _lock > 0ul;
}