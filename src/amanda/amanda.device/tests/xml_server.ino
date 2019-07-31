#include <SPI.h>
#include <SD.h>
#include <UIPEthernet.h>

#include <server/ethernet.h>
#include <server/request.h>
#include <server/storage/sdcard.h>
#include <common/data/flash_stream.h>

#include <hardware/hardware_controller.h>

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
	protected: virtual bool request_body(HTTPClientRequest request) override
	{
		request.inquire_request();

		for (unsigned int i = 0; i < content_length; )
		{
			char c = request.current();

			Serial.print(c);
			log.print(c);

			if (++i >= content_length) break;
			request.next();
		}

		Serial.println();
		log.println();

		return true;
	}
};

HardwareController& default_hw_controller()
{
	HardwareController* hw_ctrl = nullptr;
	return *hw_ctrl;
}

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

	if (!handler.parse(client)) client.respond_bad_request();
	
	client.stop();

	Serial.println(F("client disconnected"));
}
