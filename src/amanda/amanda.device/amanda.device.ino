#include <SPI.h>
#include <SD.h>
#include <UIPEthernet.h>

#include <system.h>
#include <structures/specialized/vlist.h>

#include "hardware/hardware_controller.h"
#include "hardware/events.h"

#include "server/events/event_parser.h"
#include "server/events/event_handler.h"
#include "server/ethernet.h"
#include "server/request.h"
#include "server/storage/sdcard.h"

#include "common/data/flash_stream.h"
#include "common/communication/serial_monitor.h"

using communication::SerialMonitor;

HardwareController controller;
HTTPEthernetServer server;

BTN btn1(32);
BTN btn2(34);

LED led1(22);
LED led2(23);
LED led3(24);

LMP lmp(7);

BUZZ buzz1(33);
BUZZ buzz2(35);

POT pot(A7);

LM35 lm35(A10);

LDR ldr(A15);

PIR pir(36);

void setup()
{
	System::init();

	System::lock();
	
	SerialMonitor::begin();
	pinMode(13, OUTPUT);

	if (!storage::SDCard::init()) _LOG(F("STORAGE"), F("Failed to initialize SD card."));

	EventHandler& e_handler = EventHandler::instance();
	//if (!e_handler.clean_storage()) _LOG(F("EVENTS"), F("Failed to clean the storage."));
	e_handler.init_storage();

	_LOG(F("EVENTS"), F("Storage initialized at \""), e_handler.EV_ROOT_DIR, F("\" with system information at \""), e_handler.EV_SYS_INFO, F("\""));
	_LOG(F("EVENTS"), F("Designator continuing from: "), e_handler.designator());

	ethernet::begin(IPAddress(192, 168, 56, 177));
	server.begin();
	server.bind(controller);

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

	e_handler.load_events(*server.get_bound_controller());

	System::unlock();
}

void loop()
{
	// this should run in a seperate thread, meaning these 2 threads have to be synchronized
	do EventHandler::instance().check_events(); while (server.await());

	//_LOG(F("SERVER"), F("Incoming client request"));
	HTTPClientRequest client = server.get_request();
	
	RequestHandler handler;
	if (!handler.parse(client)) client.respond_bad_request();

	client.stop();
	//_LOG(F("SERVER"), F("Client request processed"));
}

//*/
