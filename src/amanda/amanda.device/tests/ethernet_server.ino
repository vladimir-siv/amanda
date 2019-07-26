#include <SPI.h>
#include <UIPEthernet.h>

#include <server/ethernet.h>
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
			"<h1>Hello from Arduino Web Server</h1>\n"
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

	ethernet::begin(IPAddress(192, 168, 56, 177 ));
	server.begin();

	Serial.print(F("Running server at: "));
	Serial.println(ethernet::localIP());
}

void loop(void)
{
	while (server.await()) ;

	Serial.println();
	Serial.println(F("============= New client request ============="));
	Serial.println();

	HTTPClientRequest client = server.get_request();
	client.inquire_request();

	while (!client.eos())
	{
		Serial.print(client.current());
		client.next();
	}

	client.respond(data::FlashStream(Controller::index()));
	delay(10);
	client.stop();

	Serial.println(F("client disconnected"));
}
