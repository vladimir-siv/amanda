#include <Arduino.h>
#include <exceptions.h>
#include <memory_management/object_allocator.h>
#include "system.h"
#include "schedulers/fifo.h"
#include "thread.h"

unsigned long Thread::idGen = 0;
Thread Thread::loop;
Thread* Thread::running = &Thread::loop;
bool Thread::dispatch_idle = true;
Thread* Thread::idle = nullptr;
vlist<Tuple<Time, Thread*>> Thread::sleeping;

#define __push_context__() __asm volatile ("push r1\npush r0\nin r0, 0x3f\npush r0\npush r2\npush r3\npush r4\npush r5\npush r6\npush r7\npush r8\npush r9\npush r10\npush r11\npush r12\npush r13\npush r14\npush r15\npush r16\npush r17\npush r18\npush r19\npush r20\npush r21\npush r22\npush r23\npush r24\npush r25\npush r26\npush r27\npush r28\npush r29\npush r30\npush r31\nin r0, 0x1e\npush r0\nin r0, 0x2a\npush r0\nin r0, 0x2b\npush r0\n")
#define __pop_context__() __asm volatile ("pop r0\nout 0x2b, r0\npop r0\nout 0x2a, r0\npop r0\nout 0x1e, r0\npop r31\npop r30\npop r29\npop r28\npop r27\npop r26\npop r25\npop r24\npop r23\npop r22\npop r21\npop r20\npop r19\npop r18\npop r17\npop r16\npop r15\npop r14\npop r13\npop r12\npop r11\npop r10\npop r9\npop r8\npop r7\npop r6\npop r5\npop r4\npop r3\npop r2\npop r0\nout 0x3f, r0\npop r0\npop r1\n")

ObjectAllocator<Tuple<Time, Thread*>> tuplespace;
Tuple<Time, Thread*>* newTuple(Time time, Thread* thread)
{
	auto tuple = tuplespace.alloc();
	tuple->e<0>() = time;
	tuple->e<1>() = thread;
	return tuple;
}
void deleteTuple(Tuple<Time, Thread*>* tuple)
{
	tuplespace.dealloc(tuple);
}

volatile bool inSwitching = false;
volatile bool puttingToSleep = false;
volatile unsigned long long ticks = 0;

void Thread::init()
{
	idle = new Thread([](void) -> void
	{
		while (true)
		{
			if (Thread::dispatch_idle) dispatch();
		}
	});
}

extern "C" void __attribute__((signal, __INTR_ATTRS)) __attribute__((naked)) TIMER1_COMPA_vect(void)
{
	__push_context__();

	++System::_millis;
	Thread::tick();

	if (!inSwitching)
	{
		if (Thread::running->quantum > 0) --Thread::running->quantum;

		if (!System::locked() && Thread::running->quantum <= 0)
		{
			if (!Thread::running->isInState(Thread::FINISHED))
			{
				Thread::running->sp = SP;
				if (!Thread::running->isInState(Thread::WAITING) && Thread::running != Thread::idle) System::scheduler->put(Thread::running);
				if (Thread::running->isInState(Thread::RUNNING)) Thread::running->setState(Thread::READY);
			}

			while (true)
			{
				Thread::running = System::scheduler->get();
				if (Thread::running == nullptr || !Thread::running->isInState(Thread::ABORTING)) break;
				Thread::running->finish();
			}

			if (Thread::running == nullptr) { Thread::running = Thread::idle; Thread::dispatch_idle = false; }
			Thread::running->setState(Thread::RUNNING);
			SP = Thread::running->sp;
		}
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
		if (!Thread::running->isInState(Thread::FINISHED))
		{
			Thread::running->sp = SP;
			if (!Thread::running->isInState(Thread::WAITING) && Thread::running != Thread::idle) System::scheduler->put(Thread::running);
			if (Thread::running->isInState(Thread::RUNNING)) Thread::running->setState(Thread::READY);
		}

		while (true)
		{
			Thread::running = System::scheduler->get();
			if (Thread::running == nullptr || !Thread::running->isInState(Thread::ABORTING)) break;
			Thread::running->finish();
		}
		
		if (Thread::running == nullptr) { Thread::running = Thread::idle; Thread::dispatch_idle = false; }
		Thread::running->setState(Thread::RUNNING);
		SP = Thread::running->sp;
	}

	inSwitching = false;

	__pop_context__();
	__asm volatile ("reti\n");
}

void Thread::tick()
{
	++ticks;
	if (puttingToSleep) return;

	for (; ticks > 0; --ticks)
	{
		if (sleeping.size() > 0)
		{
			Tuple<Time, Thread*>* tuple = nullptr;

			--sleeping.peek_front()->e<0>();
			while (sleeping.size() > 0 && (tuple = sleeping.peek_front())->e<0>() == 0)
			{
				Thread* thread = tuple->e<1>();
				sleeping.remove_front();
				deleteTuple(tuple); tuple = nullptr;
				thread->setState(Thread::READY);
				System::scheduler->put(thread);
			}

			deleteTuple(tuple);
		}
	}
}

Thread* Thread::current()
{
	return running;
}

void Thread::sleep(Time millis)
{
	System::lock();

	if (millis > 0)
	{
		Thread* current = Thread::current();

		puttingToSleep = true;

		auto i = sleeping.begin();
		auto end = sleeping.end();
		
		auto prev = end;

		for (; i != end; prev = i, ++i)
		{
			Time& current = (*i)->e<0>();
			
			if (millis < current)
			{
				current -= millis;
				break;
			}

			millis -= current;
		}

		current->setState(Thread::WAITING);
		if (prev == end) sleeping.push_front(newTuple(millis, current));
		else prev.insertAfter(newTuple(millis, current));

		puttingToSleep = false;

		System::unlock();
		dispatch();
	}
	else System::unlock();
}

Thread::Thread() : id(idGen++), /*stack(nullptr),*/ sp(0), quantum(Thread::_DEFAULT_QUANTUM), state((State)(State::RUNNING | State::LOOP))
{

}

Thread::Thread(ThreadDelegate delegate, unsigned long stackSize) : Thread(delegate, nullptr, stackSize)
{

}

Thread::Thread(ThreadDelegate delegate, void* context, unsigned long stackSize) : Thread()
{
	System::lock();

	this->context = context;
	this->state = State::READY;

#ifndef _FIXED_STACK_SIZE_
	if (stackSize < 128)
	{
		this->setState(FINISHED);
		Exceptions::Throw<InsufficientStackSizeException>();
		return;
	}

	stack = new byte[stackSize];
#else
	stackSize = _STACK_SIZE;
#endif

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
	System::scheduler->put(this);

	System::unlock();
}

Thread::~Thread()
{
	this->waitToComplete();
}

void Thread::finish()
{
#ifndef _FIXED_STACK_SIZE_
	delete[] this->stack;
	this->stack = nullptr;
#endif
	this->setState(Thread::FINISHED);

	while (complete.size() > 0)
	{
		Thread* thread = complete.pop_front();
		thread->setState(Thread::READY);
		System::scheduler->put(thread);
	}
}

void  __attribute__((naked)) __attribute__((noinline)) Thread::finalize()
{
	System::lock();
	
	Thread* next = nullptr;

	while (true)
	{
		next = System::scheduler->get();
		if (next == nullptr || !next->isInState(Thread::ABORTING)) break;
		next->finish();
	}

	if (next == nullptr) { next = idle; dispatch_idle = false; }
	next->setState(Thread::RUNNING);
	SP = next->sp;

	Thread::running->finish();
	Thread::running = next;

	System::unlock();

	__pop_context__();
	__asm volatile ("reti\n");
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
	if (state & Thread::LOOP) return;
	this->state = (State)(this->state & (Thread::ABORTING | Thread::FINISHED | Thread::LOOP));
	this->state = (State)(this->state | state);
}

void Thread::waitToComplete()
{
	System::lock();

	if (!this->isInState(Thread::FINISHED) && this != running)
	{
		running->setState(Thread::WAITING);
		this->complete.push_back(running);
		System::unlock();
		dispatch();
	}
	else System::unlock();
}

void Thread::abort()
{
	System::lock();

	if (this == running)
	{
		System::unlock();
		finalize(); // ends execution of the current (running) thread here
	}
	else
	{
		this->state = (State)(this->state | Thread::ABORTING);
		System::unlock();
	}
}
