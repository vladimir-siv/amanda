#include <SPI.h>
#include <UIPEthernet.h>

extern const int _ETH_CS;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 56, 177);

EthernetServer server(80);

void _setup_ethernet_server(void)
{
	pinMode(_ETH_CS, OUTPUT);

	Serial.println(F("Ethernet WebServer Test"));

	Ethernet.begin(mac, ip);

	server.begin();

	Serial.print(F("Server is at "));
	Serial.println(Ethernet.localIP());
}

const char* const page =
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
	"</html>\n";

void _check_for_ethernet_client(void)
{
	EthernetClient client = server.available();

	if (client)
	{
		Serial.println();
		Serial.println(F("============= New client request ============="));
		Serial.println();

		bool currentLineIsBlank = true;
		while (client.connected())
		{
			if (!client.available()) continue;

			char c = client.read();
			Serial.write(c);

			if (c == '\n' && currentLineIsBlank)
			{
				client.write((uint8_t*)page, strlen(page));
				break;
			}

			if (c == '\n') currentLineIsBlank = true;
			else if (c != '\r') currentLineIsBlank = false;
		}

		delay(10);
		client.stop();
		Serial.println(F("client disconnected"));
	}
}
