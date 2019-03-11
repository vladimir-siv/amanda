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
	Serial.flush();

	pinMode(13, OUTPUT);
	
	System::init();
	
	if (N > 0)
	{
		t = new Thread*[N];

		System::lock();

		for (byte i = 0; i < N; ++i)
		{
			t[i] = new Thread(&thread, 121 + i);

			Serial.print(Exceptions::Count());
			Serial.print(" ->");
			
			if (Exceptions::Count() > 0)
			{
				while (Exceptions::Count() > 0)
				{
					Serial.print(" { ");
					Serial.print(Exceptions::Fetch()->what());
					Serial.print(" }");
				}

				Serial.println();
			}
			else Serial.println(" <no exception thrown>");
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
			Serial.print("Aborting t[7]: ");
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
			Serial.print("Waiting for: ");
			Serial.println(t[i]->ID());
			System::unlock();
			t[i]->waitToComplete();
		}

		firstCycle = false;
	}

	job(7);

	pin13state = !pin13state;
	digitalWrite(13, pin13state);

	static int k = 0;
	if (++k == 5)
	{
		for (int i = 0; i < N; ++i) delete t[i];
		Serial.println();
		run();
		Serial.println("End.");
		Thread::current()->abort();
	}
}

void thread()
{
	job(8);
}

// ============== TEST 2 ============== //

#include <synchronization/mutex.h>
#include <synchronization/lock.h>

mutex mtx;

void print(const char* str)
{
	static int i = 0;
	//lock lck(mtx);

	Serial.print("[");
	Serial.print(++i);
	Serial.print("] ");
	Serial.println(str);
}

void noncriticalcode()
{
	for (volatile int i = 0; i < 1000; i++)
		for (volatile int j = 0; j < 1000; j++)
			;
}

void run()
{

}
