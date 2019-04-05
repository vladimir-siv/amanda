#include "hardware/components/btn.h"
#include "hardware/components/led.h"
#include "hardware/components/lmp.h"
#include "hardware/components/buzz.h"
#include "hardware/components/pot.h"
#include "hardware/components/lm35.h"
#include "hardware/components/ldr.h"

#include <system.h>

BTN* btn1;
BTN* btn2;
LED* led1;
LED* led2;
LED* led3;
LMP* lmp1;
BUZZ* buzz1;
BUZZ* buzz2;
POT* pot1;
LM35* lm351;
LDR* ldr1;

void setup()
{
	Serial.begin(9600);
	while (!Serial) ;
	Serial.flush();

	pinMode(13, OUTPUT);

	System::init();

	System::lock();

	btn1 = new BTN(32);
	btn2 = new BTN(34);
	
	led1 = new BlinkingLED(12);
	led2 = new LED(11);
	led3 = new LED(10);

	lmp1 = new BlinkingLMP(7);

	buzz1 = new BUZZ(33);
	buzz2 = new BUZZ(35);

	//pot1 = new POT(A7);

	//lm351 = new LM35(A10);

	ldr1 = new LDR(A15);

	System::unlock();

	led1->execute(F("<?xml version=\"1.0\" encoding=\"UTF-8\"?><command name=\"blink\"><arg>1000</arg></command>"));
	led2->execute(F("<?xml version=\"1.0\" encoding=\"UTF-8\"?><command name=\"blink\"><arg>1000</arg></command>"));
	led3->execute(F("<?xml version=\"1.0\" encoding=\"UTF-8\"?><command name=\"blink\"><arg>1000</arg></command>"));

	lmp1->execute(F("<?xml version=\"1.0\" encoding=\"UTF-8\"?><command name=\"blink\"><arg>10000</arg></command>"));

	buzz1->play(523);
}

void loop()
{
	Serial.print(F("btn1: "));
	Serial.println(btn1->read() == HIGH ? F("HIGH") : F("LOW"));

	Serial.print(F("btn2: "));
	Serial.println(btn2->read() == HIGH ? F("HIGH") : F("LOW"));

	/*Serial.print(F("pot1: "));
	Serial.print(pot1->read());
	Serial.println(pot1->unit());*/

	/*Serial.print(F("lm351: "));
	Serial.print(lm351->read());
	Serial.println(lm351->unit());*/

	Serial.print(F("ldr1: "));
	Serial.print(ldr1->read());
	Serial.println(ldr1->unit());

	Serial.println();

	Thread::sleep(1000);
}
