#include <system.h>

#include "hardware/hardware_controller.h"
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
		controller[2]->execute(*cmd);
		controller[3]->execute(*cmd);
		controller[4]->execute(*cmd);
	}

	cmd = CommandParser::parse("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><command name=\"blink\"><arg>10000</arg></command>");
	if (cmd != nullptr)
	{
		controller[5]->execute(*cmd);
	}
}

void loop()
{
	System::lock();
	Serial.print(F("<scan>"));
	System::unlock();

	auto& scan = controller.scan();

	System::lock();
	//scan.print_to_serial();
	Serial.println(F("</scan>"));
	System::unlock();

	Thread::sleep(1000);
}
//*/
