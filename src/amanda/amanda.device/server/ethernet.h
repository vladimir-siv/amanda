#pragma once

#include <Arduino.h>
#include <SPI/src/SPI.h>
#include <UIPEthernet/UIPEthernet.h>

#include <thread.h>

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

	class HTTPClient final : public data::InputStream
	{
		friend class HTTPServer;

		private: HTTPClient(const HTTPClient&) = default;
		private: HTTPClient(HTTPClient&&) = default;
		private: HTTPClient& operator=(const HTTPClient&) = default;
		private: HTTPClient& operator=(HTTPClient&&) = default;
		
		private: EthernetClient _client;
		private: char c;
		
		private: HTTPClient() : _client(), c(0) { }
		public: virtual ~HTTPClient() { /* stop(); */ }
		
		private: HTTPClient& operator=(EthernetClient client)
		{
			_client = client;
			c = 0;
			return *this;
		}
		public: operator bool() const { return const_cast<EthernetClient&>(_client); }
		
		public: void inquire_request() { next(); }
		public: void stop() { _client.stop(); }
		
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
		
		public: size_t respond(uint8_t data) { return _client.write(data); }
		public: size_t respond(const uint8_t* buf, size_t size) { return _client.write(buf, size); }
		
		public: void respond(data::InputStream& stream)
		{
			while (!stream.eos())
			{
				respond((uint8_t)stream.advance());
			}
		}
		public: void respond(data::InputStream&& stream) { respond(stream); }
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

	class HTTPRequestParser
	{
		private: enum State { UNKNOWN = 0, REQUEST_URI = 1, REQUEST_HEADER = 2, REQUEST_BODY = 4 };
		
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
				if (state != REQUEST_BODY && chr == '\r') continue;

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
							advanceState(chr, REQUEST_BODY);
							continue;
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
					case REQUEST_BODY:
					{
						if (!request_body(chr)) return false;
					} break;

					default: return false;
				}
				
				// This class does not know even if the request body exists,
				// certainly not how to handle one. This is why the subclass
				// needs to tell if the parsing of request body is done.
				if (state == REQUEST_BODY && request_body_end()) break;

				prev = chr;
			}

			return !_cancel;
		}
		
		protected: void cancel() { _cancel = true; }
		
		protected: virtual void reset() { }
		protected: virtual void request_uri(const char* uri) = 0;
		protected: virtual void request_header(const char* hname, const char* hvalue) = 0;
		protected: virtual void request_header_end() { }
		protected: virtual bool request_body_end() const { return true; }
		protected: virtual bool request_body(char chr) { return false; }
	};
}
