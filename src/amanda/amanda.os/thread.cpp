#include <Arduino.h>
#include "system.h"
#include "exceptions.h"
#include "schedulers/fifo.h"
#include "thread.h"

template<> Scheduler<Thread>* Scheduler<Thread>::instance = new Scheduler<Thread>(10);
unsigned long Thread::idGen = 0;
Thread Thread::loop;
Thread* Thread::running = &Thread::loop;
volatile bool inSwitching = false;

#define __push_context__() __asm volatile ("push r1\npush r0\nin r0, 0x3f\npush r0\npush r2\npush r3\npush r4\npush r5\npush r6\npush r7\npush r8\npush r9\npush r10\npush r11\npush r12\npush r13\npush r14\npush r15\npush r16\npush r17\npush r18\npush r19\npush r20\npush r21\npush r22\npush r23\npush r24\npush r25\npush r26\npush r27\npush r28\npush r29\npush r30\npush r31\nin r0, 0x1e\npush r0\nin r0, 0x2a\npush r0\nin r0, 0x2b\npush r0\n")
#define __pop_context__() __asm volatile ("pop r0\nout 0x2b, r0\npop r0\nout 0x2a, r0\npop r0\nout 0x1e, r0\npop r31\npop r30\npop r29\npop r28\npop r27\npop r26\npop r25\npop r24\npop r23\npop r22\npop r21\npop r20\npop r19\npop r18\npop r17\npop r16\npop r15\npop r14\npop r13\npop r12\npop r11\npop r10\npop r9\npop r8\npop r7\npop r6\npop r5\npop r4\npop r3\npop r2\npop r0\nout 0x3f, r0\npop r0\npop r1\n")

extern "C" void __attribute__((signal, __INTR_ATTRS)) __attribute__((naked)) TIMER1_COMPA_vect(void)
{
	__push_context__();

	++System::_millis;
	if (Thread::running->quantum > 0) --Thread::running->quantum;

	if (!inSwitching && !System::locked() && Thread::running->quantum <= 0)
	{
		if (!Thread::running->isInState((Thread::State)(Thread::ABORTING | Thread::FINISHED)))
		{
			Thread::running->sp = SP;
			Scheduler<Thread>::I()->put(Thread::running);
			if (Thread::running->isInState(Thread::RUNNING)) Thread::running->setState(Thread::READY);
		}

		Thread::running = Scheduler<Thread>::I()->get();
		Thread::running->setState(Thread::RUNNING);
		SP = Thread::running->sp;

		Thread::running->quantum = Thread::_DEFAULT_QUANTUM;
	}

	__pop_context__();
	__asm volatile ("reti\n");
}

void __attribute__((naked)) __attribute__((noinline)) dispatch()
{
	__push_context__();

	inSwitching = true;

	if (!System::locked())
	{
		if (!Thread::running->isInState((Thread::State)(Thread::ABORTING | Thread::FINISHED)))
		{
			Thread::running->sp = SP;
			Scheduler<Thread>::I()->put(Thread::running);
			if (Thread::running->isInState(Thread::RUNNING)) Thread::running->setState(Thread::READY);
		}

		Thread::running = Scheduler<Thread>::I()->get();
		Thread::running->setState(Thread::RUNNING);
		SP = Thread::running->sp;

		Thread::running->quantum = Thread::_DEFAULT_QUANTUM;
	}

	inSwitching = false;

	__pop_context__();
	__asm volatile ("ret\n");
}

Thread* Thread::current()
{
	return running;
}

Thread::Thread() : id(idGen++), stack(nullptr), sp(0), quantum(Thread::_DEFAULT_QUANTUM), state((State)(State::RUNNING | State::LOOP))
{

}

Thread::Thread(ThreadDelegate delegate, unsigned long stackSize = 128) : Thread()
{
	this->state = State::READY;

	if (stackSize < 128)
	{
		Exceptions::Throw<InsufficientStackSizeException>();
		return;
	}

	stack = new byte[stackSize];

	const uint_farptr_t _delegate = (uint_farptr_t)delegate;
	const uint_farptr_t _finalize = (uint_farptr_t)(&finalize);
	stack[stackSize - 1] = (byte)(_finalize >> 0);
	stack[stackSize - 2] = (byte)(_finalize >> 8);
	stack[stackSize - 3] = (byte)(_finalize >> 16);
	stack[stackSize - 4] = (byte)(_delegate >> 0);
	stack[stackSize - 5] = (byte)(_delegate >> 8);
	stack[stackSize - 6] = (byte)(_delegate >> 16);
	stack[stackSize - 7] = 0x00; // r1
	stack[stackSize - 8] = 0x00; // r0
	stack[stackSize - 9] = 0x80; // status register (I bit set is 0x80)

	this->sp = (uintptr_t)(stack + stackSize - 43);
	Scheduler<Thread>::I()->put(this);
}

Thread::~Thread()
{
	// await();
}

void Thread::clear()
{
	delete[] this->stack;
	this->stack = nullptr;
	this->setState(Thread::FINISHED);
}

void  __attribute__((naked)) __attribute__((noinline)) Thread::finalize()
{
	System::lock();
	
	Thread* next = Scheduler<Thread>::I()->get();
	SP = next->sp;

	Thread::running->clear();
	Thread::running = next;

	Thread::running->setState(Thread::RUNNING);
	Thread::running->quantum = Thread::_DEFAULT_QUANTUM;

	__pop_context__();

	System::unlock();

	__asm volatile ("ret\n");
}

unsigned long Thread::ID() const
{
	return this->id;
}

bool Thread::isInState(State state) const
{
	return this->state & state;
}

void Thread::setState(State state)
{
	if (state & State::LOOP) return;
	this->state = (State)(this->state & State::LOOP);
	this->state = (State)(this->state | state);
}
