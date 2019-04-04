#include "hardware/components/btn.h"
#include "hardware/components/led.h"
#include "hardware/components/lmp.h"

#include <system.h>

BTN* btn1;
BTN* btn2;
LED* led1;
LED* led2;
LED* led3;
LMP* lmp1;

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

	led1->execute("<?xml version=\"1.0\" encoding=\"UTF-8\"?><blink><arg>1000</arg></blink>");
	led2->execute("<?xml version=\"1.0\" encoding=\"UTF-8\"?><blink><arg>1000</arg></blink>");
	led3->execute("<?xml version=\"1.0\" encoding=\"UTF-8\"?><blink><arg>1000</arg></blink>");

	lmp1->execute("<?xml version=\"1.0\" encoding=\"UTF-8\"?><blink><arg>10000</arg></blink>");

	System::unlock();
}

void loop()
{
	Serial.print("btn1: ");
	Serial.println(btn1->read() == HIGH ? F("HIGH") : F("LOW"));
	Serial.print("btn2: ");
	Serial.println(btn2->read() == HIGH ? F("HIGH") : F("LOW"));

	Thread::sleep(1000);
}
