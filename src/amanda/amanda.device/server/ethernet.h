#pragma once

#include <Arduino.h>
#include <SPI/src/SPI.h>
#include <UIPEthernet/UIPEthernet.h>

#include "../common/data/stream.h"

#define HTTPClientRequest ethernet::HTTPClient&

class MACAddress final
{
	private: byte mac[6];

	public: MACAddress(byte b1, byte b2, byte b3, byte b4, byte b5, byte b6) :
		mac { b1, b2, b3, b4, b5, b6 } { }
	public: MACAddress(const byte* raw) :
		MACAddress(raw[0], raw[1], raw[2], raw[3], raw[4], raw[5]) { }
	
	public: const byte* raw() const { return mac; }
	public: MACAddress& operator=(const byte* raw)
	{
		for (unsigned int i = 0; i < 6; ++i)
		{
			mac[i] = raw[i];
		}

		return *this;
	}
};

namespace ethernet
{
	void begin(IPAddress host, MACAddress mac = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }, uint8_t _cs_pin = 53, uint8_t _ws_pin = 10);
	IPAddress localIP();
	IPAddress gatewayIP();
	IPAddress dnsServerIP();
	IPAddress subnetMask();
	int maintain();

	class HTTPClient final : public data::Stream
	{
		friend class HTTPServer;

		private: HTTPClient(const HTTPClient&) = default;
		private: HTTPClient(HTTPClient&&) = default;
		private: HTTPClient& operator=(const HTTPClient&) = default;
		private: HTTPClient& operator=(HTTPClient&&) = default;
		
		private: EthernetClient _client;
		private: char c;
		private: unsigned int lfcnt;
		private: bool done;
		
		private: HTTPClient() : _client(), c(0), lfcnt(0), done(false) { }
		public: virtual ~HTTPClient() { /* stop(); */ }
		
		private: HTTPClient& operator=(EthernetClient client)
		{
			_client = client;
			c = 0;
			lfcnt = 0;
			done = false;
			return *this;
		}
		public: operator bool() const { return const_cast<EthernetClient&>(_client); }
		
		public: void inquire_request() { next(); }
		public: void stop() { _client.stop(); }
		
		public: uint8_t connected() const { return const_cast<EthernetClient&>(_client).connected(); }
		
		public: virtual char current() const override
		{
			return c;
		}
		public: virtual void next() override
		{
			if (!eos())
			{
				while (_client.available() == 0) ; // is this a good idea?
				c = (char)_client.read();

				if (c == '\n') ++lfcnt;
				else if (c != '\r') lfcnt = 0;

				if (lfcnt == 2) done = true;
			}
		}
		public: virtual bool eos() const override
		{
			return !connected() || done;
		}
		
		public: void flush() { _client.flush(); }
		
		public: size_t respond(uint8_t data) { return _client.write(data); }
		public: size_t respond(const uint8_t* buf, size_t size) { return _client.write(buf, size); }
		
		public: void respond(data::Stream& stream)
		{
			while (!stream.eos())
			{
				respond((uint8_t)stream.advance());
			}
		}
		public: void respond(data::Stream&& stream) { respond(stream); }
	};

	class HTTPServer
	{
		protected: EthernetServer server;
		protected: HTTPClient client;
		
		public: explicit HTTPServer(uint16_t port = 80) : server(port) { }
		public: virtual ~HTTPServer() { }
		
		public: void begin() { server.begin(); }
		public: bool await() { return !(client = server.available()); }
		public: HTTPClient& get_request() const { return (HTTPClient&)client; }
		public: size_t write(uint8_t data) { return server.write(data); }
		public: size_t write(const uint8_t* buf, size_t size) { return server.write(buf, size); }
	};
}
