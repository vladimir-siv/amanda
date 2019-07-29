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
	private: bool firstchar;
	
	public: Req2Hndl() : log(F("/request.log")), firstchar(true) { }
	public: virtual ~Req2Hndl() { }
	
	protected: virtual void request_uri(const char* uri) override
	{
		Serial.print('\'');
		Serial.print(uri);
		Serial.println('\'');
		RequestHandler::request_uri(uri);
	}
	protected: virtual void request_header(const char* hname, const char* hvalue) override
	{
		Serial.print('\'');
		Serial.print(hname);
		Serial.print(F(": "));
		Serial.print(hvalue);
		Serial.println('\'');
		RequestHandler::request_header(hname, hvalue);
	}
	protected: virtual void request_header_end() override
	{
		Serial.println();
		RequestHandler::request_header_end();
	}
	protected: virtual bool request_body_end() const override
	{
		bool body_end = RequestHandler::request_body_end();
		if (body_end) Serial.println('\'');
		return body_end;
	}
	protected: virtual bool request_body(char chr) override
	{
		if (firstchar)
		{
			Serial.print('\'');
			firstchar = false;
		}

		Serial.print(chr);
		log.print(chr);
		return RequestHandler::request_body(chr);
	}
};

ethernet::HTTPServer server;

class Controller final
{
	private: Controller() { }
	#define action public: static inline response
	public: using response = const __FlashStringHelper*;
	
	action bad_req()
	{
		return F
		(
			//"HTTP/1.1 400 Bad Request\r\n"
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Connection: close\r\n"
			"\r\n"
			"<!DOCTYPE html>\r\n"
			"<html lang=\"en\">\r\n"
			"<head>\r\n"
			"<meta charset = \"UTF-8\">\r\n"
			"<title>Arduino</title>\r\n"
			"</head>\r\n"
			"<body>\r\n"
			"<h1>Error 400: Bad Request</h1>\r\n"
			"</body>\r\n"
			"</html>"
		);
	}
	action index()
	{
		return F
		(
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Connection: close\r\n"
			"\r\n"
			"<!DOCTYPE html>\r\n"
			"<html lang=\"en\">\r\n"
			"<head>\r\n"
			"<meta charset = \"UTF-8\">\r\n"
			"<title>Arduino</title>\r\n"
			"</head>\r\n"
			"<body>\r\n"
			"<h1>Hello from Arduino Responsive Server</h1>\r\n"
			"</body>\r\n"
			"</html>"
		);
	}
};

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

	if (handler.parse(client)) client.respond(data::FlashStream(Controller::index()));
	else client.respond(data::FlashStream(Controller::bad_req()));

	delay(10);
	client.stop();

	Serial.println(F("client disconnected"));
}
