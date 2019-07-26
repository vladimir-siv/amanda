#include "ethernet.h"

/**
 * _cs_pin [Chip Select, aka. Slave Select - SS pin]
 * _ws_pin [Wiznet Select]
 */
void ethernet::begin(IPAddress host, MACAddress mac = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }, uint8_t _cs_pin = 53, uint8_t _ws_pin = 10)
{
	static bool initialized = false;

	if (!initialized)
	{
		pinMode(_cs_pin, OUTPUT);
		pinMode(_ws_pin, OUTPUT);
		digitalWrite(_ws_pin, HIGH);

		Ethernet.begin(mac.raw(), host);
		
		initialized = true;
	}
}

IPAddress ethernet::localIP() { return Ethernet.localIP(); }
IPAddress ethernet::gatewayIP() { return Ethernet.gatewayIP(); }
IPAddress ethernet::dnsServerIP() { return Ethernet.dnsServerIP(); }
IPAddress ethernet::subnetMask() { return Ethernet.subnetMask(); }

int ethernet::maintain() { return Ethernet.maintain(); }
