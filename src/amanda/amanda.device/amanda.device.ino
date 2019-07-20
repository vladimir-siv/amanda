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

void test(bool _setup = false)
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

	#define newvlist(type) (D::vlists->alloc<type>(D::nodes))
	
	static cond* cnd_ldr = nullptr;
	static cond* cnd_pir = nullptr;

	static pack* pck = nullptr;
	static activity* act = nullptr;

	static event* evt = nullptr;

	static bool lastv = false;

	if (_setup)
	{
		cnd_ldr = D::sdds->alloc<cond>(sdd::cast(&ldr), sdd::cast(newvlist(comparator)));
		cnd_pir = D::sdds->alloc<cond>(sdd::cast(&pir), sdd::cast(newvlist(comparator)));

		pck = D::sdds->alloc<pack>(sdd::cast(newvlist(cond)), sdd::cast(nullptr));
		act = D::sdds->alloc<activity>(sdd::cast(false), sdd::cast(nullptr));

		evt = D::sdds->alloc<event>(sdd::cast(newvlist(pack)), sdd::cast(act));

		cnd_ldr->append("leq", 70.0f);
		cnd_pir->append("equ", HIGH);

		pck->append(cnd_ldr);
		pck->append(cnd_pir);

		evt->append(pck);
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

	const Command* cmd;

	cmd = CommandParser::parse("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><command name=\"blink\"><arg>1000</arg></command>");
	if (cmd != nullptr)
	{
		controller[3]->execute(*cmd);
		controller[4]->execute(*cmd);
		controller[5]->execute(*cmd);
	}

	cmd = CommandParser::parse("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><command name=\"blink\"><arg>10000</arg></command>");
	if (cmd != nullptr)
	{
		controller[6]->execute(*cmd);
	}

	test(true);
}

void loop()
{
	Thread::sleep(100);
	//Serial.print(F("<scan>"));
	//controller.scan(&sscanner);
	//Serial.println(F("</scan>"));
	test();
}
//*/
