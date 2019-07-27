#include <SPI.h>
#include <UIPEthernet.h>
#include <SD.h>

#include <server/ethernet.h>
#include <server/storage/sdcard.h>
#include <common/data/flash_stream.h>

ethernet::HTTPServer server;

class Controller final
{
	private: Controller() { }
	#define action public: static inline response
	public: using response = const __FlashStringHelper*;
	
	action index()
	{
		return F
		(
			"HTTP/1.1 200 OK\n"
			"Content-Type: text/html\n"
			"Connection: close\n"
			"\n"
			"<!DOCTYPE html>\n"
			"<html lang=\"en\">\n"
			"<head>\n"
			"<meta charset = \"UTF-8\">\n"
			"<title>Arduino</title>\n"
			"</head>\n"
			"<body>\n"
			"<h1>Hello from Arduino Storage Server</h1>\n"
			"</body>\n"
			"</html>\n"
		);
	}
};

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

	ethernet::begin(IPAddress(192, 168, 56, 177));
	server.begin();

	Serial.print(F("Running server at: "));
	Serial.println(ethernet::localIP());
}

void loop(void)
{
	while (server.await()) ;
	HTTPClientRequest client = server.get_request();

	Serial.println();
	Serial.println(F("============= New client request ============="));
	Serial.println();

	SD.remove(F("/request.log"));
	client.inquire_request();
	storage::SDCard::WriteToFile(F("/request.log"), client);
	storage::SDCard::PrintFileToSerial(F("/request.log"));

	client.respond(data::FlashStream(Controller::index()));
	delay(10);
	client.stop();

	Serial.println(F("client disconnected"));
}
