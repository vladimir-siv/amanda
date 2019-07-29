#include <SPI.h>
#include <UIPEthernet.h>
#include <SD.h>

#include <server/ethernet.h>
#include <server/request.h>
#include <server/storage/sdcard.h>
#include <common/data/flash_stream.h>

class Req2Hndl : public RequestHandler
{
	private: storage::OutputFileStream log;
	
	public: Req2Hndl() : log(F("/request.log")) { }
	public: virtual ~Req2Hndl() { }
	
	protected: virtual void request_uri(const char* uri) override
	{
		Serial.print('\'');
		Serial.print(uri);
		Serial.println('\'');

		log.print('\'');
		log.print(uri);
		log.println('\'');

		RequestHandler::request_uri(uri);
	}
	protected: virtual void request_header(const char* hname, const char* hvalue) override
	{
		Serial.print('\'');
		Serial.print(hname);
		Serial.print(F(": "));
		Serial.print(hvalue);
		Serial.println('\'');

		log.print('\'');
		log.print(hname);
		log.print(F(": "));
		log.print(hvalue);
		log.println('\'');

		RequestHandler::request_header(hname, hvalue);
	}
	protected: virtual void request_header_end() override
	{
		Serial.println();
		log.println();
		RequestHandler::request_header_end();
	}
};

ethernet::HTTPServer server;

void setup(void)
{
	Serial.begin(9600);
	while (!Serial);
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
	Req2Hndl handler;

	if (!handler.parse(client)) client.bad_request();

	delay(10);
	client.stop();

	Serial.println(F("client disconnected"));
}
