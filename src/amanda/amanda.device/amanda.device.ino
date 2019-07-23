#include <assert/static.h>
#include <system.h>

#include "hardware/hardware_controller.h"
#include "hardware/events.h"
#include "server/commands/command_parser.h"

HardwareController controller;

BTN btn1(32);
BTN btn2(34);

BlinkingLED led1(12);
LED led2(11);
LED led3(10);

BlinkingLMP lmp(7);

BUZZ buzz1(33);
BUZZ buzz2(35);

POT pot(A7);

LM35 lm35(A10);

LDR ldr(A15);

PIR pir(36);

SerialScanner sscanner;

void test()
{
	class Base
	{
		public: Base() { }
		public: virtual ~Base() { }
		public: virtual void method() { Serial.println(F("Base")); }
	};

	class Test : public Base
	{
		private: static const unsigned long long _newid()
		{
			static unsigned long long gid = 0;
			return ++gid;
		}
		public: const unsigned long long id = _newid();
		
		public: Test() { Serial.println(F(".ctor()")); }
		public: Test(const Test&) { Serial.println(F(".copy()")); }
		public: Test(Test&&) { Serial.println(F(".move()")); }
		public: virtual ~Test() { Serial.println(F(".dtor()")); }
		public: Test& operator=(const Test&) { Serial.println(F(".copy=")); return *this; }
		public: Test& operator=(Test&&) { Serial.println(F(".move=")); return *this; }
		
		public: virtual void method() override { Serial.println(F("Test")); }
	};
}

void test_events(bool _setup = false)
{
	auto __oncall = []()
	{
		static unsigned long c = 0;
		++c;
		if (c < 10) Serial.print(F("    "));
		else if (c < 100) Serial.print(F("   "));
		else if (c < 1000) Serial.print(F("  "));
		else if (c < 10000) Serial.print(F(" "));
		Serial.print(c);
		Serial.print(F(":"));
	};
	//__oncall();

	static event* evt = nullptr;
	static pack* pck = nullptr;
	static cond* cnd_ldr = nullptr;
	static cond* cnd_pir = nullptr;
	static action* raise = nullptr;
	static action* expire = nullptr;

	static bool lastv = false;

	if (_setup)
	{
		cnd_ldr = cond::_new(&ldr)->compare("leq", 70.0f);
		cnd_pir = cond::_new(&pir)->compare("equ", HIGH);
		
		pck = pack::_new();
		pck->append(cnd_ldr);
		pck->append(cnd_pir);

		raise = action::_new(&led3, HIGH);
		expire = action::_new(&led3, LOW);

		evt = event::_new(3);

		evt->append(pck);
		evt->appendRaise(raise);
		evt->appendExpire(expire);
	}

	bool newv = evt->check();
	if (newv != lastv) Serial.println(newv);
	lastv = newv;
}

void setup()
{
	Serial.begin(9600);
	while (!Serial) ;
	Serial.flush();

	pinMode(13, OUTPUT);
	System::init();

	System::lock();
	
	controller += &btn1;
	controller += &btn2;
	
	controller += &led1;
	controller += &led2;
	controller += &led3;
	
	controller += &lmp;
	
	controller += &buzz1;
	controller += &buzz2;

	controller += &pot;

	controller += &lm35;
	
	controller += &ldr;

	controller += &pir;
	
	System::unlock();

	Command& cmd = CommandParser::instance().extractCommand();

	bool p1 = CommandParser::instance().parse("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><command name=\"blink\"><arg>1000</arg></command>");
	if (p1)
	{
		controller[3]->execute(cmd);
		controller[4]->execute(cmd);
		controller[5]->execute(cmd);
	}

	bool p2 = CommandParser::instance().parse("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><command name=\"blink\"><arg>10000</arg></command>");
	if (p2)
	{
		controller[6]->execute(cmd);
	}

	if (!p1) Serial.println(F("P1 failed."));
	if (!p2) Serial.println(F("P2 failed."));

	test();
	test_events(true);
}

void loop()
{
	Thread::sleep(100);
	//Serial.print(F("<scan>"));
	//controller.scan(&sscanner);
	//Serial.println(F("</scan>"));
	test_events();
}
//*/
