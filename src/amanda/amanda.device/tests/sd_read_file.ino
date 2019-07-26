#include <SPI.h>
#include <SD.h>

#include <server/storage/sdcard.h>

void setup(void)
{
	Serial.begin(9600);
	while (!Serial) ;
	Serial.flush();

	if (!storage::SDCard::init())
	{
		pinMode(13, OUTPUT);
		while (true)
		{
			digitalWrite(13, HIGH); delay(500);
			digitalWrite(13, LOW); delay(500);
		}
	}

	storage::SDCard::PrintFileToSerial(F("core.xsa"));

	Serial.println();
	Serial.println(F("[~EOS:core.xsa]"));
}

void loop(void) { }
