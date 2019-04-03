#include <SPI.h>
#include <SD.h>

#include "xml/tinyxml.h"

extern const int _SD_CS;

void _sd_xml(void)
{
	auto filepath = F("utf8t1.xml");

	pinMode(_SD_CS, OUTPUT);

	Serial.print(F("Initializing SD card..."));

	if (!SD.begin(_SD_CS))
	{
		Serial.println(F(" Initialization failed!"));
		while (1) ;
	}
	Serial.println(F(" Initialization done."));

	File file = SD.open(filepath);
	if (file)
	{
		char* data = new char[file.size() + 1];
		
		unsigned int i;
		for (i = 0; file.available(); ++i) data[i] = (char)file.read();
		data[i] = 0;

		TiXmlDocument doc;
		doc.Parse(data, 0, TIXML_ENCODING_UTF8);

		const char* text = doc.RootElement()->FirstChildElement()->GetText();
		Serial.println(text);

		delete data;
		file.close();
	}
	else
	{
		Serial.print(F("Error opening "));
		Serial.println(filepath);
	}
}
