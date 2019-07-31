#include <SPI.h>
#include <SD.h>
#include <UIPEthernet.h>

#include <system.h>
#include <structures/specialized/vlist.h>

#include "hardware/hardware_controller.h"
#include "hardware/events.h"

#include "server/commands/command_parser.h"
#include "server/events/event_parser.h"
#include "server/events/event_handler.h"
#include "server/ethernet.h"
#include "server/request.h"
#include "server/storage/sdcard.h"

#include "common/data/flash_stream.h"
#include "common/communication/serial_monitor.h"

using communication::SerialMonitor;
#define _LOG(TAG, ...) SerialMonitor::println(F(" > ["), TAG, F("] {@"), (unsigned long)System::millis(), F("ms} "), ##__VA_ARGS__)

//#define _RUN_TESTS

HardwareController controller;
HTTPEthernetServer server;

BTN btn1(32);
BTN btn2(34);

BlinkingLED led1(22);
LED led2(23);
LED led3(24);

BlinkingLMP lmp(7);

BUZZ buzz1(33);
BUZZ buzz2(35);

POT pot(A7);

LM35 lm35(A10);

LDR ldr(A15);

PIR pir(36);

HardwareController& default_hw_controller()
{
	return controller;
}

void setup()
{
	System::init();

	System::lock();
	
	SerialMonitor::begin();
	pinMode(13, OUTPUT);
	if (!storage::SDCard::init()) _LOG(F("STORAGE"), F("Failed to initialize SD card."));
	EventHandler::instance().init_storage();
	ethernet::begin(IPAddress(192, 168, 56, 177));
	server.begin();

	_LOG(F("SERVER"), F("HTTP server running at: "), ethernet::localIP());

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

#ifdef _RUN_TESTS
	test_setup();
#endif
}

void loop()
{
#ifndef _RUN_TESTS
	while (server.await())
	{
		// this should run in a seperate thread, meaning these 2 threads have to be synchronized
		EventHandler::instance().check_events();
	}

	_LOG(F("SERVER"), F("Incoming client request"));
	HTTPClientRequest client = server.get_request();
	
	RequestHandler handler;
	if (!handler.parse(client)) client.respond_bad_request();

	client.stop();
	_LOG(F("SERVER"), F("Client request processed"));
#else
	test_loop();
#endif
}

// ===================== //
// ======= TESTS ======= //
// ===================== //

#ifdef _RUN_TESTS

void test_commands(bool _setup = false)
{
	if (_setup)
	{
		Command& cmd = CommandParser::instance().extractCommand();

		bool p1 = CommandParser::instance().parse(data::FlashStream(F("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><command name=\"blink\"><arg>1000</arg></command>")));
		if (p1)
		{
			_LOG(F("EXECUTE"), cmd.name.c_str(), '(', cmd.args[0].c_str(), F(", "), cmd.args[1].c_str(), F(", "), cmd.args[2].c_str(), F(", "), cmd.args[3].c_str(), F(", "), cmd.args[4].c_str(), ')');
			controller[3]->execute(cmd);
			controller[4]->execute(cmd);
			controller[5]->execute(cmd);
		}

		bool p2 = CommandParser::instance().parse(data::FlashStream(F("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><command name=\"blink\"><arg>10000</arg></command>")));
		if (p2)
		{
			_LOG(F("EXECUTE"), cmd.name.c_str(), '(', cmd.args[0].c_str(), F(", "), cmd.args[1].c_str(), F(", "), cmd.args[2].c_str(), F(", "), cmd.args[3].c_str(), F(", "), cmd.args[4].c_str(), ')');
			controller[6]->execute(cmd);
		}

		if (!p1) SerialMonitor::println(F("P1 failed."));
		if (!p2) SerialMonitor::println(F("P2 failed."));
	}
}
void test_events(bool _setup = false)
{
	auto __oncall = []()
	{
		static unsigned long c = 0;
		++c;
		if (c < 10) SerialMonitor::print(F("    "));
		else if (c < 100) SerialMonitor::print(F("   "));
		else if (c < 1000) SerialMonitor::print(F("  "));
		else if (c < 10000) SerialMonitor::print(F(" "));
		SerialMonitor::print(c);
		SerialMonitor::print(F(":"));
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
		_LOG(F("EVENTS"), F("Setting up..."));

		cnd_ldr = cond::_new(&ldr)->compare("leq", 100.0f);
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

		_LOG(F("EVENTS"), F("Done!"));
	}

	bool newv = evt->check();
	if (newv != lastv) SerialMonitor::println(newv);
	lastv = newv;
}
void test_event_parser(bool _setup = false)
{
	static event* evt = nullptr;
	static bool lastv = false;

	if (_setup)
	{
		_LOG(F("EVENTS"), F("Setting up..."));

		EventParser parser(&controller);
		bool p = parser.parse(data::FlashStream(F("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><event repeat=\"3\"><requirements><pack><condition vid=\"4\" ctype=\"AS\"><lss>100.0</lss></condition><condition vid=\"3\" ctype=\"DS\"><equ>1</equ></condition></pack></requirements><actions><raise><write vid=\"3\" ctype=\"DE\"><state>1</state></write></raise><expire><write vid=\"3\" ctype=\"DE\"><state>0</state></write></expire></actions></event>")));

		if (p) evt = parser.extractEvent();
		else SerialMonitor::println(F("Parsing event failed."));

		_LOG(F("EVENTS"), F("Done!"));
	}

	if (evt)
	{
		bool newv = evt->check();
		if (newv != lastv) SerialMonitor::println(newv);
		lastv = newv;
	}
}

void test_setup()
{
	//test_commands(true);
	//test_events(true);
	//test_event_parser(true);
}

void test_loop()
{
	//SerialScanner sscanner;
	//SerialMonitor::print(F("<scan>"));
	//controller.scan(&sscanner);
	//SerialMonitor::println(F("</scan>"));

	//test_commands();
	//test_events();
	//test_event_parser();
}

#endif

//*/
