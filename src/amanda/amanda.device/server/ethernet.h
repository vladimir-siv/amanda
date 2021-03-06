#pragma once

#include <Arduino.h>
#include <SPI/src/SPI.h>
//#include <UIPEthernet/UIPEthernet.h>
#include <Ethernet/src/Ethernet.h>

#include <thread.h>

#include "../hardware/hardware_controller.h"

#include "../common/data/stream.h"
#include "../common/data/flash_stream.h"

#define HTTPClientRequest ethernet::HTTPClient&
#define HTTPEthernetServer ethernet::HTTPServer

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
	class HTTPServer;

	void begin(IPAddress host, MACAddress mac = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }, uint8_t _cs_pin = 53, uint8_t _ws_pin = 10);
	IPAddress localIP();
	IPAddress gatewayIP();
	IPAddress dnsServerIP();
	IPAddress subnetMask();
	int maintain();

	class HTTPClient final : public data::InputStream, public data::OutputStream
	{
		friend class HTTPServer;

		private: HTTPClient(const HTTPClient&) = default;
		private: HTTPClient(HTTPClient&&) = default;
		private: HTTPClient& operator=(const HTTPClient&) = default;
		private: HTTPClient& operator=(HTTPClient&&) = default;
		
		private: HTTPServer* _server;
		private: EthernetClient _client;
		private: char c;
		private: bool header_sent;
		
		private: HTTPClient(HTTPServer* server) : _server(server), _client(), c(0), header_sent(false) { }
		public: virtual ~HTTPClient() { /* stop(); */ }
		
		private: HTTPClient& operator=(EthernetClient client)
		{
			_client = client;
			c = 0;
			header_sent = false;
			return *this;
		}
		public: operator bool() const { return const_cast<EthernetClient&>(_client); }
		
		public: HTTPServer* get_handling_server() const { return _server; }
		
		public: void inquire_request() { next(); }
		public: void stop() { respond_blank(); Thread::delay(10); _client.stop(); }
		
		public: uint8_t connected() const { return const_cast<EthernetClient&>(_client).connected(); }
		public: int available() const { return const_cast<EthernetClient&>(_client).available(); }
		
		public: virtual char current() const override
		{
			return c;
		}
		public: virtual void next() override
		{
			if (!eos())
			{
				while (_client.available() == 0) dispatch(); // is this a good idea?
				c = (char)_client.read();
			}
		}
		public: virtual bool eos() const override
		{
			// As long as the clinet is connected, he may send data
			return !connected();
		}
		
		public: void flush() { _client.flush(); }
		
		private: void send_header()
		{
			if (header_sent) return;
			header_sent = true;

			data::FlashStream fs
			(
				F
				(
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: application/xml\r\n"
					"Connection: close\r\n"
					"\r\n"
					"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>"
				)
			);

			System::lock();
			while (!fs.eos()) _client.write(fs.advance());
			System::unlock();
		}
		
		public: size_t respond(uint8_t data)
		{
			send_header();
			System::lock();
			auto s = _client.write(data);
			System::unlock();
			return s;
		}
		public: size_t respond(const uint8_t* buf, size_t size)
		{
			send_header();
			System::lock();
			auto s = _client.write(buf, size);
			System::unlock();
			return s;
		}
		
		public: size_t respond(char data) { return respond((uint8_t)data); }
		public: size_t respond(const char* str) { return respond((const uint8_t*)str, strlen(str)); }
		
		public: size_t respond(unsigned long data, int base = DEC)
		{
			if (data == 0) return respond('0');

			size_t s = 0;

			char digits[64] = { };
			byte len = 0;

			while (data > 0)
			{
				digits[len++] = '0' + data % base;
				data /= base;
			}

			while (len > 0) s += respond(digits[--len]);

			return s;
		}
		public: size_t respond(long data, int base = DEC)
		{
			if (data == 0) return respond('0');

			size_t s = 0;

			if (data < 0)
			{
				data *= -1;
				respond('-');
			}

			char digits[64] = { };
			byte len = 0;

			while (data > 0)
			{
				digits[len++] = '0' + data % base;
				data /= base;
			}

			while (len > 0) s += respond(digits[--len]);

			return s;
		}
		
		public: size_t respond(double data)
		{
			if (data == 0.0) return respond('0') + respond('.') + respond('0') + respond('0');

			size_t s = 0;

			s +=
				respond((long)data, 10)
				+
				respond('.');

			if (data < 0) data *= -1;
			data *= 100.0;
			long point = round(data);
			//if (point < 0) point *= -1;

			point %= 100;

			s += respond((char)('0' + point / 10));
			s += respond((char)('0' + point % 10));

			return s;
		}
		
		public: size_t respond(data::InputStream& stream)
		{
			size_t s = 0;

			while (!stream.eos())
			{
				s += respond(stream.advance());
			}

			return s;
		}
		public: size_t respond(data::InputStream&& stream) { return respond(stream); }
		
		public: size_t respond_crlf() { return respond('\r') + respond('\n'); }
		
		public: void respond_bad_request()
		{
			if (header_sent) return;
			header_sent = true;

			data::FlashStream fs
			(
				F
				(
					//"HTTP/1.1 400 Bad Request\r\n"
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: application/xml\r\n"
					"Connection: close\r\n"
					"\r\n"
					"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>\r\n"
					"<response>\r\n"
					"\t<meta>\r\n"
					"\t\t<status>400</status>\r\n"
					"\t\t<title>Arduino</title>\r\n"
					"\t</meta>\r\n"
					"\t<content>\r\n"
					"\t\t<message>Error 400: Bad Request</message>\r\n"
					"\t</content>\r\n"
					"</response>"
				)
			);

			System::lock();
			while (!fs.eos()) _client.write(fs.advance());
			System::unlock();
		}
		public: void respond_blank()
		{
			if (header_sent) return;
			header_sent = true;

			data::FlashStream fs
			(
				F
				(
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: application/xml\r\n"
					"Connection: close\r\n"
					"\r\n"
					"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>\r\n"
					"<response>\r\n"
					"\t<meta>\r\n"
					"\t\t<status>200</status>\r\n"
					"\t\t<title>Arduino</title>\r\n"
					"\t</meta>\r\n"
					"\t<content>\r\n"
					"\t</content>\r\n"
					"</response>"
				)
			);

			System::lock();
			while (!fs.eos()) _client.write(fs.advance());
			System::unlock();
		}
		
		// OutputStream INTERFACE
		
		public: virtual size_t print(const __FlashStringHelper* data) override
		{
			data::FlashStream fs(data);

			size_t s = 0;

			while (!fs.eos()) s += respond(fs.advance());

			return s;
		}
		public: virtual size_t print(const String& data) override { return respond((const uint8_t*)data.c_str(), data.length()); }
		public: virtual size_t print(const char data[]) override { return respond(data); }
		public: virtual size_t print(char data) override { return respond(data); }
		public: virtual size_t print(unsigned char data, int base = DEC) override { return respond((unsigned long)data, base); }
		public: virtual size_t print(int data, int base = DEC) override { return respond((long)data, base); }
		public: virtual size_t print(unsigned int data, int base = DEC) override { return respond((unsigned long)data, base); }
		public: virtual size_t print(long data, int base = DEC) override { return respond(data, base); }
		public: virtual size_t print(unsigned long data, int base = DEC) override { return respond(data, base); }
		public: virtual size_t print(double data, int base = 2) override { return respond(data); }
		
		public: virtual size_t println(const __FlashStringHelper* data) override { return print(data) + respond_crlf(); }
		public: virtual size_t println(const String& data) override { return print(data) + respond_crlf(); }
		public: virtual size_t println(const char data[]) override { return print(data) + respond_crlf(); }
		public: virtual size_t println(char data) override { return print(data) + respond_crlf(); }
		public: virtual size_t println(unsigned char data, int base = DEC) override { return print(data, base) + respond_crlf(); }
		public: virtual size_t println(int data, int base = DEC) override { return print(data, base) + respond_crlf(); }
		public: virtual size_t println(unsigned int data, int base = DEC) { return print(data, base) + respond_crlf(); }
		public: virtual size_t println(long data, int base = DEC) override { return print(data, base) + respond_crlf(); }
		public: virtual size_t println(unsigned long data, int base = DEC) override { return print(data, base) + respond_crlf(); }
		public: virtual size_t println(double data, int base = 2) override { return print(data) + respond_crlf(); }
		public: virtual size_t println(void) override { return respond_crlf(); }
		
		public: virtual size_t write(uint8_t data) override { return respond(data); }
		public: virtual size_t write(const char* str) override { return respond(str); }
		public: virtual size_t write(const char* buffer, size_t size) override { return respond((const uint8_t*)buffer, size); }
		public: virtual size_t write(const uint8_t* buffer, size_t size) override { return respond(buffer, size); }
	};

	class HTTPServer
	{
		protected: EthernetServer server;
		protected: HTTPClient client;
		protected: HardwareController* controller;
		
		public: explicit HTTPServer(uint16_t port = 80) : server(port), client(this), controller(nullptr) { }
		public: virtual ~HTTPServer() { }
		
		public: void begin() { server.begin(); }
		public: void bind(HardwareController& controller) { this->controller = &controller; }
		public: HardwareController* get_bound_controller() const { return controller; }
		
		public: bool await() { return !(client = server.available()); }
		public: HTTPClient& get_request() const { return (HTTPClient&)client; }
		public: size_t write(uint8_t data) { return server.write(data); }
		public: size_t write(const uint8_t* buf, size_t size) { return server.write(buf, size); }
	};

	class HTTPRequestParser
	{
		private: enum State { UNKNOWN = 0, REQUEST_URI = 1, REQUEST_HEADER = 2, DONE = 4 };
		
		private: bool _cancel;
		private: char buffer1[65] = { };
		private: char buffer2[65] = { };
		
		public: virtual ~HTTPRequestParser() { }
		
		public: bool parse(HTTPClientRequest request)
		{
			State state = REQUEST_URI;
			unsigned int c = 0;
			char prev = 0;
			bool colon = false;

			auto advanceState = [&](char chr, State next) -> bool
			{
				if (chr != '\n') return false;

				state = next;
				c = 0;
				prev = 0;
				colon = false;

				return true;
			};
			auto buff = [&](char* buffer, char chr)
			{
				if (c < 64) buffer[c++] = chr != '\n' ? chr : 0;
			};
			auto is_whitespace = [](char chr) -> bool
			{
				return chr == ' ' || chr == '\t';
			};

			_cancel = false;
			reset();

			for (request.inquire_request(); !request.eos() && !_cancel; request.next())
			{
				char chr = request.current();
				if (chr == '\r') continue;

				switch (state)
				{
					case REQUEST_URI:
					{
						if (chr == '\n' && c == 0) return false;

						buff(buffer1, chr);
						
						if (advanceState(chr, REQUEST_HEADER))
						{
							request_uri(buffer1);
							continue;
						}
					} break;
					case REQUEST_HEADER:
					{
						bool isChrColon = chr == ':';
						bool isChrNewLine = chr == '\n';
						bool isCntZero = c == 0;

						if (isChrNewLine && !colon && c == 0)
						{
							request_header_end();

							state = DONE;
							c = 0;
							prev = 0;
							colon = false;

							if (!request_body(request)) return false;
							goto end;
						}
						
						if (isChrColon && isCntZero) return false;
						if (isChrNewLine && isCntZero) return false;
						if (isChrNewLine && !colon) return false;

						bool isChrWhitespace = is_whitespace(chr);
						bool isPrevWhitespace = is_whitespace(prev);

						if (isChrWhitespace)
						{
							chr = ' ';
							if (isCntZero) continue;
							if (isPrevWhitespace) continue;
						}

						if (isPrevWhitespace)
						{
							if (isChrColon) buffer1[c - 1] = 0;
							if (isChrNewLine) buffer2[c - 1] = 0;
						}

						if (isChrColon)
						{
							if (colon) return false;

							buff(buffer1, 0);
							c = 0;
							prev = 0;
							colon = true;
							continue;
						}

						if (colon) buff(buffer2, chr);
						else buff(buffer1, chr);

						if (advanceState(chr, REQUEST_HEADER))
						{
							request_header(buffer1, buffer2);
							continue;
						}
					} break;

					default: return false;
				}

				prev = chr;
			}

		end:
			return !_cancel;
		}
		
		protected: void cancel() { _cancel = true; }
		
		protected: virtual void reset() { }
		protected: virtual void request_uri(const char* uri) = 0;
		protected: virtual void request_header(const char* hname, const char* hvalue) = 0;
		protected: virtual void request_header_end() { }
		protected: virtual bool request_body(HTTPClientRequest request) { }
	};
}
