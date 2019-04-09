#include <exceptions.h>
#include <system.h>
#include <thread.h>

const byte N = 10;
Thread** t = nullptr;
pinstate pin13state = LOW;
bool firstCycle = true;

void thread();

void setup()
{
	Serial.begin(9600);
	while (!Serial) ;
	Serial.flush();

	pinMode(13, OUTPUT);
	
	System::init();

	t = new Thread*[N];
	for (byte i = 0; i < N; ++i) t[i] = nullptr;

	if (N > 0)
	{
		System::lock();

		for (byte i = 0; i < N; ++i)
		{
			t[i] = new Thread(&thread, 121 + i);

			Serial.print(Exceptions::Count());
			Serial.print(F(" ->"));
			
			if (Exceptions::Count() > 0)
			{
				while (Exceptions::Count() > 0)
				{
					Serial.print(F(" { "));
					Serial.print(Exceptions::Fetch()->what());
					Serial.print(F(" }"));
				}

				Serial.println();
			}
			else Serial.println(F(" <no exception thrown>"));
		}

		System::unlock();
	}
}

void job(int id)
{
	for (int i = 0; i < 10; ++i)
	{
		System::lock();
		Serial.print(Thread::current()->ID());
		Serial.print('[');
		Serial.print(id);
		Serial.print(']');
		Serial.println();
		System::unlock();

		if (i < 5) dispatch();

		// Non-Critical code
		for (volatile int i = 0; i < 1000 / (N > 0 ? N : 1); i++)
			for (volatile int j = 0; j < 1000; j++)
				;

		System::lock();

		if (i == 3 && Thread::current() == t[8])
		{
			Serial.print(F("Aborting t[7]: "));
			Serial.println(t[7]->ID());
			t[7]->abort();
		}

		System::unlock();
	}
}

void loop()
{
	if (firstCycle)
	{
		for (int i = 0; i < N; ++i)
		{
			System::lock();
			Serial.print(F("Waiting for: "));
			Serial.println(t[i]->ID());
			System::unlock();
			if (t[i] != nullptr) t[i]->waitToComplete();
		}

		firstCycle = false;
	}

	job(7);

	pin13state = !pin13state;
	digitalWrite(13, pin13state);

	static int k = 0;
	if (++k == 5)
	{
		for (int i = 0; i < N; ++i)
		{
			delete t[i];
			t[i] = nullptr;
		}

		test2();
		test3();
		test4();
		test5();

		Serial.println();
		Serial.println(F("========== Finished Tests =========="));
		Serial.println();

		cli();

		Time msms = System::millis();
		unsigned long msts = millis();

		sei();

		Serial.print(F("System:  "));
		Serial.println((unsigned long)msms);
		Serial.print(F("Arduino: "));
		Serial.println(msts);
		
		Serial.println();

		Serial.println(F("End."));
		Thread::current()->abort();
	}
}

void thread()
{
	job(8);
}

void noncriticalcode(int k = 1)
{
	for (volatile int i = 0; i < 1000 / k; i++)
		for (volatile int j = 0; j < 1000; j++)
			;
}

// ============== TEST 2 ============== //

#include <synchronization/mutex.h>
#include <synchronization/lock.h>

mutex mtx;

void print(const char* str)
{
	lock lck(mtx);

	static int i = 0;
	Serial.print('[');
	Serial.print(++i);
	Serial.print(F("] "));
	Serial.println(str);
}

void test2()
{
	Serial.println();
	Serial.println(F("========== Test2 =========="));
	Serial.println();

	auto fast = []() -> void
	{
		unsigned long id = Thread::current()->ID();
		char signature[4] { '0', '0', 'F', 0 };
		signature[0] += id / 10;
		signature[1] += id % 10;

		for (int i = 0; i < 10; ++i)
		{
			print(signature);
			print(signature);
			print(signature);
			noncriticalcode(500);
		}

		System::lock();
		Serial.print(id);
		Serial.println(F(" -> end."));
		System::unlock();
	};
	auto slow = []() -> void
	{
		unsigned long id = Thread::current()->ID();
		char signature[4] { '0', '0', 'S', 0 };
		signature[0] += id / 10;
		signature[1] += id % 10;

		for (int i = 0; i < 10; ++i)
		{
			print(signature);
			noncriticalcode(100);
		}

		System::lock();
		Serial.print(id);
		Serial.println(F(" -> end."));
		System::unlock();
	};

	System::lock();

	t[0] = new Thread(slow);
	t[1] = new Thread(slow);
	t[2] = new Thread(fast);
	t[3] = new Thread(fast);
	t[4] = new Thread(fast);

	System::unlock();

	for (byte i = 0; i < N; ++i)
	{
		if (t[i] == nullptr) continue;
		System::lock();
		Serial.print(F("Waiting for: "));
		Serial.println(t[i]->ID());
		System::unlock();
		//t[i]->waitToComplete();
		delete t[i];
		t[i] = nullptr;
	}
}

// ============== TEST 3 ============== //

void test3()
{
	Serial.println();
	Serial.println(F("========== Test3 =========="));
	Serial.println();

	Serial.println(F("Check1"));
	Thread::sleep(5000);
	Serial.println(F("Check2"));
	Thread::sleep(2500);
	Serial.println(F("Check3"));

	auto type1 = []() -> void
	{
		ID id = Thread::current()->ID();

		System::lock();
		Serial.print(F("Type 1 started: "));
		Serial.print((const unsigned long)id);
		Serial.print(F(" ["));
		Serial.print((const unsigned long)id * 250);
		Serial.print(F("ms]"));
		Serial.println();
		System::unlock();

		Thread::sleep((Time)id * 250);

		System::lock();
		Serial.print(F("Type 1 passed: "));
		Serial.print((const unsigned long)id);
		Serial.print(F(" ["));
		Serial.print((const unsigned long)id * 125);
		Serial.print(F("ms]"));
		Serial.println();
		System::unlock();

		Thread::sleep((Time)id * 125);

		System::lock();
		Serial.print(F("Type 1 finished: "));
		Serial.print((const unsigned long)id);
		Serial.println();
		System::unlock();
	};
	auto type2 = []() -> void
	{
		ID id = Thread::current()->ID();

		System::lock();
		Serial.print(F("Type 2 started: "));
		Serial.print((const unsigned long)id);
		Serial.print(F(" ["));
		Serial.print((const unsigned long)id * 100);
		Serial.print(F("ms]"));
		Serial.println();
		System::unlock();

		Thread::sleep(id * 100);

		System::lock();
		Serial.print(F("Type 2 passed: "));
		Serial.print((const unsigned long)id);
		Serial.print(F(" ["));
		Serial.print((const unsigned long)id * 50);
		Serial.print(F("ms]"));
		Serial.println();
		System::unlock();

		Thread::sleep(id * 50);

		System::lock();
		Serial.print(F("Type 2 finished: "));
		Serial.print((const unsigned long)id);
		Serial.println();
		System::unlock();
	};

	System::lock();

	t[0] = new Thread(type1);
	t[1] = new Thread(type1);
	t[2] = new Thread(type2);
	t[3] = new Thread(type2);
	t[4] = new Thread(type2);

	System::unlock();

	for (byte i = 0; i < N; ++i)
	{
		if (t[i] == nullptr) continue;
		System::lock();
		Serial.print(F("Waiting for: "));
		Serial.println(t[i]->ID());
		System::unlock();
		//t[i]->waitToComplete();
		delete t[i];
		t[i] = nullptr;
	}
}

// ============== TEST 4 ============== //

#include <synchronization/condition.h>

condition cnd;

void test4()
{
	Serial.println();
	Serial.println(F("========== Test4 =========="));
	Serial.println();

	auto master = []() -> void
	{
		System::lock();
		Serial.println(F("MASTER start"));
		System::unlock();

		noncriticalcode();
		noncriticalcode();
		noncriticalcode();

		System::lock();
		Serial.println(F("Notifying one slave"));
		cnd.notify();
		System::unlock();

		noncriticalcode();
		noncriticalcode();
		noncriticalcode();

		System::lock();
		Serial.println(F("Notifying all slaves"));
		cnd.notify_all();
		System::unlock();

		System::lock();
		Serial.println(F("MASTER end."));
		System::unlock();
	};
	auto slave = []() -> void
	{
		System::lock();
		Serial.print(F("Slave "));
		Serial.print(Thread::current()->ID());
		Serial.println(F(" blocked?."));
		System::unlock();

		cnd.wait();

		System::lock();
		Serial.print(F("Slave "));
		Serial.print(Thread::current()->ID());
		Serial.println(F(" passed."));
		System::unlock();
	};

	System::lock();

	t[0] = new Thread(master);
	t[1] = new Thread(slave);
	t[2] = new Thread(slave);
	t[3] = new Thread(slave);
	t[4] = new Thread(slave);

	System::unlock();

	for (byte i = 0; i < N; ++i)
	{
		if (t[i] == nullptr) continue;
		System::lock();
		Serial.print(F("Waiting for: "));
		Serial.println(t[i]->ID());
		System::unlock();
		//t[i]->waitToComplete();
		delete t[i];
		t[i] = nullptr;
	}
}

// ============== TEST 5 ============== //

#include <synchronization/semaphore.h>

semaphore sem;

void test5()
{
	Serial.println();
	Serial.println(F("========== Test5 =========="));
	Serial.println();

	auto master = []() -> void
	{
		System::lock();
		Serial.println(F("MASTER start"));
		System::unlock();

		noncriticalcode();
		noncriticalcode();
		noncriticalcode();

		System::lock();
		Serial.println(F("Notifying one slave"));
		sem.notify();
		System::unlock();

		noncriticalcode();
		noncriticalcode();
		noncriticalcode();

		System::lock();
		Serial.println(F("Notifying all slaves"));
		sem.notify_all();
		System::unlock();

		System::lock();
		Serial.println(F("MASTER end."));
		System::unlock();
	};
	auto slave = []() -> void
	{
		System::lock();
		Serial.print(F("Slave "));
		Serial.print(Thread::current()->ID());
		Serial.println(F(" blocked?."));
		System::unlock();

		sem.wait();

		System::lock();
		Serial.print(F("Slave "));
		Serial.print(Thread::current()->ID());
		Serial.println(F(" passed."));
		System::unlock();
	};

	System::lock();

	t[0] = new Thread(master);
	t[1] = new Thread(slave);
	t[2] = new Thread(slave);
	t[3] = new Thread(slave);
	t[4] = new Thread(slave);

	System::unlock();

	for (byte i = 0; i < N; ++i)
	{
		if (t[i] == nullptr) continue;
		System::lock();
		Serial.print(F("Waiting for: "));
		Serial.println(t[i]->ID());
		System::unlock();
		//t[i]->waitToComplete();
		delete t[i];
		t[i] = nullptr;
	}
}
