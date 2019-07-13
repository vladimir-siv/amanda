#include <exceptions.h>
#include <system.h>
#include <thread.h>
#include <RAII/generic.h>

namespace logging
{
	class FLogEntry
	{
		public: virtual ~FLogEntry() { }
		public: virtual size_t print() = 0;
	};

	class LogEntry
	{
		protected: union letype
		{
			const __FlashStringHelper* v1;
			const char* v2;
			char v3;
			unsigned char v4;
			int v5;
			unsigned int v6;
			long v7;
			unsigned long v8;
			double v9;
			FLogEntry* v10;
		};
		
		protected: letype content;
		protected: byte type;
		
		public: LogEntry() : type(0) { }
		public: virtual ~LogEntry() { }
		
		public: void set(const __FlashStringHelper* v1) { content.v1 = v1; type = 1; }
		public: void set(const char v2[]) { content.v2 = v2; type = 2; }
		public: void set(char v3) { content.v3 = v3; type = 3; }
		public: void set(unsigned char v4) { content.v4 = v4; type = 4; }
		public: void set(int v5) { content.v5 = v5; type = 5; }
		public: void set(unsigned int v6) { content.v6 = v6; type = 6; }
		public: void set(long v7) { content.v7 = v7; type = 7; }
		public: void set(unsigned long v8) { content.v8 = v8; type = 8; }
		public: void set(double v9) { content.v9 = v9; type = 9; }
		public: void set(FLogEntry* v10) { content.v10 = v10; type = 10; }
		
		public: virtual size_t print()
		{
			switch (type)
			{
				case 1: return Serial.print(content.v1);
				case 2: return Serial.print(content.v2);
				case 3: return Serial.print(content.v3);
				case 4: return Serial.print(content.v4);
				case 5: return Serial.print(content.v5);
				case 6: return Serial.print(content.v6);
				case 7: return Serial.print(content.v7);
				case 8: return Serial.print(content.v8);
				case 9: return Serial.print(content.v9);
				case 10: return content.v10->print();
			}

			return 0;
		}
	};

	class NLLogEntry : public FLogEntry
	{
		public: static FLogEntry* instance() { static NLLogEntry inst; return &inst; }
		private: NLLogEntry() { }
		public: virtual ~NLLogEntry() { }
		public: virtual size_t print() override { return Serial.println(); }
	};

	class Log final
	{
		private: Log() { }
		
		private: static const unsigned int SIZE = 512;
		private: static LogEntry logs[SIZE];
		private: static unsigned int top(bool increment = true)
		{
			static unsigned int val = 0;
			unsigned int v = val;
			if (increment) ++val;
			return v;
		}
		
		public: static FLogEntry* endl() { return NLLogEntry::instance(); }
		
		private: static bool _add() { return true; }
		private: template <typename T, typename... Types> static bool _add(T&& value, Types&&... values)
		{
			if (top(false) >= SIZE) return false;
			logs[top()].set(std::forward<T>(value));
			return _add(std::forward<Types>(values)...);
		}
		
		public: template <typename... Types> static bool add(Types&&... values)
		{
			RAII::Light e([] { System::unlock(); }, [] { System::lock(); });
			return _add(std::forward<Types>(values)...);
		}
		public: template <typename... Types> static bool addln(Types&&... values)
		{
			RAII::Light e([] { System::unlock(); }, [] { System::lock(); });
			return _add(std::forward<Types>(values)..., endl());
		}
		
		public: static void print()
		{
			for (unsigned int i = 0; i < top(false); ++i)
			{
				logs[i].print();
			}
		}
		
		public: static unsigned int debug() { return top(false); }
	};

	LogEntry Log::logs[SIZE] = { };
};
namespace communication
{
	class SerialMonitor final
	{
		private: static const bool PRINT = true;
		
		private: SerialMonitor() { }
		
		public: class Functional final
		{
			friend class SerialMonitor;
			public: using Delegate = void(*)();
			
			private: Delegate del;
			private: Functional(Delegate del) : del(del) { }
			public: Functional& operator()() { del(); return *this; }
			public: const Functional& operator()() const { del(); return *this; }
		};
		public: static Functional endl;
		
		public: static void begin(unsigned long baud = 9600)
		{
			Serial.begin(baud);
			while (!Serial);
			Serial.flush();
		}
		
		private: template <typename T> static void _print(T&& param) { if (PRINT) Serial.print(std::forward<T>(param)); }
		private: static void _print(Functional& param) { param(); }
		
		private: static void _pass() { }
		private: template <typename T, typename... Types> static void _pass(T&& param, Types&&... params)
		{
			_print(std::forward<T>(param));
			_pass(std::forward<Types>(params)...);
		}
		
		public: template <typename... Types> static void print(Types&&... params)
		{
			System::lock();
			_pass(std::forward<Types>(params)...);
			System::unlock();
		}
		public: template <typename... Types> static void println(Types&&... params)
		{
			System::lock();
			_pass(std::forward<Types>(params)...);
			if (PRINT) Serial.println();
			System::unlock();
		}
	};

	SerialMonitor::Functional SerialMonitor::endl([](void) -> void { if (PRINT) Serial.println(); });
}

using namespace logging;
using namespace communication;

namespace workers
{
	class Workers final
	{
		private: Workers() { }
		
		private: class Worker final
		{
			friend class Workers;

			private: Thread worker;
			private: volatile bool sync;
			
			private: Worker(unsigned int id, unsigned long stackSize) :
				worker([] { Workers::work(); }, (void*)id, stackSize),
				sync(false)
			{ }
			
			private: bool isFinished() const
			{
				return worker.isInState(Thread::FINISHED);
			}
			private: void abort()
			{
				worker.abort();
			}
			private: void waitToComplete()
			{
				worker.waitToComplete();
			}
		};
		
		private: static Worker worker[];
		private: static ThreadDelegate job;
		
		private: static void work();
		public: static void apply(ThreadDelegate j);
		public: static void wait();
		public: static void stop();
	};
	Workers::Worker Workers::worker[]
	{
		{ 0, 128 },
		{ 1, 128 },
		{ 2, 128 },
		{ 3, 128 },
		{ 4, 128 },
		{ 5, 127 },
		{ 6, 126 },
		{ 7, 125 },
		{ 8, 124 },
		{ 9, 123 }
	};
	ThreadDelegate Workers::job = nullptr;

	void Workers::work()
	{
		unsigned int id = Thread::current_context<unsigned int>();

		while (true)
		{
			while (!worker[id].sync) ;
			if (job != nullptr) (*job)();
			worker[id].sync = false;
		}
	}
	void Workers::apply(ThreadDelegate job)
	{
		//wait();
		Workers::job = job;

		for (unsigned int i = 0; i < sizeof(worker) / sizeof(Worker); ++i)
		{
			worker[i].sync = true;
		}
	}
	void Workers::wait()
	{
		for (unsigned int i = 0; i < sizeof(worker) / sizeof(Worker); ++i)
		{
			if (worker[i].isFinished()) continue;
			while (worker[i].sync) ;
		}
	}
	void Workers::stop()
	{
		for (unsigned int i = 0; i < sizeof(worker) / sizeof(Worker); ++i)
		{
			worker[i].abort();
		}

		for (unsigned int i = 0; i < sizeof(worker) / sizeof(Worker); ++i)
		{
			worker[i].waitToComplete();
		}
	}
}

using namespace workers;

void setup()
{
	SerialMonitor::begin(9600);
	pinMode(13, OUTPUT);

	Log::addln(F("Exceptions #: "), Exceptions::Count());
	while (Exceptions::Count() > 0) Log::addln(F("{ "), Exceptions::Fetch()->what(), F(" }"));

	System::init();

	Workers::apply(&job);
	Workers::wait();
	Workers::stop();
}

void switch13()
{
	static pinstate pin13state = LOW;
	pin13state = !pin13state;
	digitalWrite(13, pin13state);
}

void noncriticalcode(int k = 1)
{
	for (volatile int i = 0; i < 1000 / k; i++)
		for (volatile int j = 0; j < 1000; j++)
			;
}

void job()
{
	for (int i = 0; i < 10; ++i)
	{
		Log::addln((unsigned long)System::millis(), F("ms: "), Thread::current()->ID());
		if (i < 5) dispatch();
		noncriticalcode(10);
	}
}

void loop()
{
	static int k = 0;

	noncriticalcode();
	switch13();

	if (++k == 5)
	{
		tests();
		dump();
		Thread::current()->abort();
	}
}

void dump()
{
	cli();

	Time msms = System::millis();
	unsigned long msts = millis();

	Log::print();
	
	Serial.println();
	Serial.print(F("Debug: "));
	Serial.println(Log::debug());

	Serial.println();
	Serial.print(F("System: "));
	Serial.println((unsigned long)msms);
	Serial.print(F("Arduino: "));
	Serial.println(msts);

	Serial.println();
	Serial.println(F("End."));

	sei();
}

// ============== TESTS ============== //

void tests()
{
	test2();
	test3();
	test4();
	test5();

	Log::addln
	(
		Log::endl(),
		F("========== Finished Tests =========="),
		Log::endl()
	);
}

unsigned int val(bool reset = false, bool fastread = false)
{
	static unsigned int value = 0;

	if (fastread) return value;

	if (reset) return value = 0;
	
	unsigned int read = value;
	noncriticalcode(100);
	value = read + 1;

	return value;
}

// ============== TEST 2 ============== //

#include <synchronization/mutex.h>
#include <synchronization/lock.h>

mutex mtx;

void test2()
{
	Log::addln
	(
		Log::endl(),
		F("========== Test2 =========="),
		Log::endl()
	);

	val(true);

	auto t = []() -> void
	{
		for (unsigned int i = 0; i < 10; ++i)
		{
			lock lck(mtx);
			val();
		}
	};

	System::lock();

	Thread t0(t);
	Thread t1(t);
	Thread t2(t);
	Thread t3(t);
	Thread t4(t);

	System::unlock();

	t0.waitToComplete();
	t1.waitToComplete();
	t2.waitToComplete();
	t3.waitToComplete();
	t4.waitToComplete();

	Log::addln(F("Result: "), val(false, true));
}

// ============== TEST 3 ============== //

void test3()
{
	Log::addln
	(
		Log::endl(),
		F("========== Test3 =========="),
		Log::endl()
	);

	Log::addln('[', (unsigned long)System::millis(), F("ms] Check1"));
	Thread::sleep(5000);
	Log::addln('[', (unsigned long)System::millis(), F("ms] Check2"));
	Thread::sleep(2500);
	Log::addln('[', (unsigned long)System::millis(), F("ms] Check3"));

	auto type1 = []() -> void
	{
		ID id = Thread::current()->ID();

		Log::addln
		(
			'[',
			(unsigned long)System::millis(),
			F("ms] Type 1 started: "),
			(const unsigned long)id,
			F(" ["),
			(const unsigned long)id * 250,
			F("ms]")
		);

		Thread::sleep((Time)id * 250);

		Log::addln
		(
			'[',
			(unsigned long)System::millis(),
			F("ms] Type 1 passed: "),
			(const unsigned long)id,
			F(" ["),
			(const unsigned long)id * 125,
			F("ms]")
		);

		Thread::sleep((Time)id * 125);

		Log::addln
		(
			'[',
			(unsigned long)System::millis(),
			F("ms] Type 1 finished: "),
			(const unsigned long)id
		);
	};
	auto type2 = []() -> void
	{
		ID id = Thread::current()->ID();

		Log::addln
		(
			'[',
			(unsigned long)System::millis(),
			F("ms] Type 2 started: "),
			(const unsigned long)id,
			F(" ["),
			(const unsigned long)id * 100,
			F("ms]")
		);

		Thread::sleep((Time)id * 100);

		Log::addln
		(
			'[',
			(unsigned long)System::millis(),
			F("ms] Type 2 passed: "),
			(const unsigned long)id,
			F(" ["),
			(const unsigned long)id * 50,
			F("ms]")
		);

		Thread::sleep((Time)id * 50);

		Log::addln
		(
			'[',
			(unsigned long)System::millis(),
			F("ms] Type 2 finished: "),
			(const unsigned long)id
		);
	};

	System::lock();

	Thread t0(type1);
	Thread t1(type1);
	Thread t2(type2);
	Thread t3(type2);
	Thread t4(type2);

	System::unlock();
}

// ============== TEST 4 ============== //

#include <synchronization/condition.h>

condition cnd;

void test4()
{
	Log::addln
	(
		Log::endl(),
		F("========== Test4 =========="),
		Log::endl()
	);

	auto master = []() -> void
	{
		Log::addln('[', (unsigned long)System::millis(), F("ms] MASTER start"));

		noncriticalcode();
		noncriticalcode();
		noncriticalcode();

		System::lock();
		Log::addln('[', (unsigned long)System::millis(), F("ms] Notifying one slave"));
		cnd.notify();
		System::unlock();

		noncriticalcode();
		noncriticalcode();
		noncriticalcode();

		System::lock();
		Log::addln('[', (unsigned long)System::millis(), F("ms] Notifying all slaves"));
		cnd.notify_all();
		System::unlock();

		Log::addln('[', (unsigned long)System::millis(), F("ms] MASTER end."));
	};
	auto slave = []() -> void
	{
		Log::addln('[', (unsigned long)System::millis(), F("ms] Slave "), Thread::current()->ID(), F(" blocked?."));
		cnd.wait();
		Log::addln('[', (unsigned long)System::millis(), F("ms] Slave "), Thread::current()->ID(), F(" passed."));
	};

	System::lock();

	Thread t0(master);
	Thread t1(slave);
	Thread t2(slave);
	Thread t3(slave);
	Thread t4(slave);

	System::unlock();
}

// ============== TEST 5 ============== //

#include <synchronization/semaphore.h>

semaphore sem;

void test5()
{
	Log::addln
	(
		Log::endl(),
		F("========== Test5 =========="),
		Log::endl()
	);

	auto master = []() -> void
	{
		Log::addln('[', (unsigned long)System::millis(), F("ms] MASTER start"));

		noncriticalcode();
		noncriticalcode();
		noncriticalcode();

		System::lock();
		Log::addln('[', (unsigned long)System::millis(), F("ms] Notifying one slave"));
		sem.notify();
		System::unlock();

		noncriticalcode();
		noncriticalcode();
		noncriticalcode();

		System::lock();
		Log::addln('[', (unsigned long)System::millis(), F("ms] Notifying all slaves"));
		sem.notify_all();
		System::unlock();

		Log::addln('[', (unsigned long)System::millis(), F("ms] MASTER end."));
	};
	auto slave = []() -> void
	{
		Log::addln('[', (unsigned long)System::millis(), F("ms] Slave "), Thread::current()->ID(), F(" blocked?."));
		sem.wait();
		Log::addln('[', (unsigned long)System::millis(), F("ms] Slave "), Thread::current()->ID(), F(" passed."));
	};

	System::lock();

	Thread t0(master);
	Thread t1(slave);
	Thread t2(slave);
	Thread t3(slave);
	Thread t4(slave);

	System::unlock();
}
