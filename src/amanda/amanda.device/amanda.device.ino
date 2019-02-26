#include <system.h>
#include <thread.h>

const byte N = 5;
Thread** t = nullptr;
state pin13state = LOW;

void thread(...);

void setup()
{
	Serial.begin(9600);
	Serial.flush();

	pinMode(13, OUTPUT);
	
	System::init();
	
	if (N > 0)
	{
		t = new Thread*[N];
		for (byte i = 0; i < N; ++i)
			t[i] = new Thread(&thread);
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
	}
}

void loop()
{
	job(7);

	pin13state = !pin13state;
	digitalWrite(13, pin13state);
}

void thread(...)
{
	job(8);
}
