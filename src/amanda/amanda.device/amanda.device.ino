#include <system.h>

#include "hardware/hardware_controller.h"
#include "server/commands/command_parser.h"

HardwareController controller;

void setup()
{
	Serial.begin(9600);
	while (!Serial) ;
	Serial.flush();

	pinMode(13, OUTPUT);

	System::init();

	System::lock();
	
	controller += new BTN(32);
	controller += new BTN(34);
	
	controller += new BlinkingLED(12);
	controller += new LED(11);
	controller += new LED(10);
	
	controller += new BlinkingLMP(7);
	
	controller += new BUZZ(33);
	controller += new BUZZ(35);

	controller += new POT(A7);

	controller += new LM35(A10);
	
	controller += new LDR(A15);

	controller += new PIR(36);
	
	System::unlock();

	/*const Command* cmd;
	
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
	}*/
}

void loop()
{
	/*Serial.print(F("<scan>"));
	controller.scan().print_to_serial();
	Serial.println(F("</scan>"));
	Thread::sleep(1000);*/
	Serial.println(F("1s"));
	delay(1000);
}
