#pragma once

#include <Arduino.h>

namespace data
{
	class InputStream
	{
		public: virtual ~InputStream() { }
		
		public: virtual char current() const = 0;
		public: virtual void next() = 0;
		public: virtual bool reset() { return false; }
		public: virtual bool eos() const { return current() == 0; }
		public: char advance() { char c = current(); next(); return c; }
	};

	class OutputStream
	{
		public: virtual ~OutputStream() { }
		
		public: virtual size_t print(const __FlashStringHelper* data) = 0;
		public: virtual size_t print(const String& data) = 0;
		public: virtual size_t print(const char data[]) = 0;
		public: virtual size_t print(char data) = 0;
		public: virtual size_t print(unsigned char data, int base = DEC) = 0;
		public: virtual size_t print(int data, int base = DEC) = 0;
		public: virtual size_t print(unsigned int data, int base = DEC) = 0;
		public: virtual size_t print(long data, int base = DEC) = 0;
		public: virtual size_t print(unsigned long data, int base = DEC) = 0;
		public: virtual size_t print(double data, int base = 2) = 0;
		
		public: virtual size_t println(const __FlashStringHelper* data) = 0;
		public: virtual size_t println(const String& data) = 0;
		public: virtual size_t println(const char data[]) = 0;
		public: virtual size_t println(char data) = 0;
		public: virtual size_t println(unsigned char data, int base = DEC) = 0;
		public: virtual size_t println(int data, int base = DEC) = 0;
		public: virtual size_t println(unsigned int data, int base = DEC) = 0;
		public: virtual size_t println(long data, int base = DEC) = 0;
		public: virtual size_t println(unsigned long data, int base = DEC) = 0;
		public: virtual size_t println(double data, int base = 2) = 0;
		public: virtual size_t println(void) = 0;
		
		public: virtual size_t write(uint8_t data) = 0;
		public: virtual size_t write(const char* str) = 0;
		public: virtual size_t write(const char* buffer, size_t size) = 0;
		public: virtual size_t write(const uint8_t* buffer, size_t size) = 0;
	};

	class RAMStream final : public InputStream
	{
		private: const char* _content;
		private: const char* _current;
		
		public: RAMStream(const char* content) : _content(content), _current(content) { }
		
		public: virtual char current() const override
		{
			return *_current;
		}
		public: virtual void next() override
		{
			if (!eos()) ++_current;
		}
		public: virtual bool reset() override
		{
			_current = _content;
			return true;
		}
	};
}
