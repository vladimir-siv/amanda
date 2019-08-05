#pragma once

#include <Arduino.h>

#include <system.h>

#include "../data/stream.h"

#define _LOG(TAG, ...) communication::SerialMonitor::println(F(" > ["), TAG, F("] {@"), (unsigned long)System::millis(), F("ms} "), ##__VA_ARGS__)

namespace communication
{
	class SerialMonitor final
	{
		private: SerialMonitor() { }
		
		public: class Functional final
		{
			friend class SerialMonitor;
			public: using Delegate = size_t (*)();
			
			private: Delegate del;
			private: Functional(Delegate del) : del(del) { }
			public: size_t operator()() { return del(); }
			public: size_t operator()() const { return del(); }
		};
		public: static Functional endl;
		
		public: static void begin(unsigned long baud = 9600)
		{
			Serial.begin(baud);
			while (!Serial) ;
			Serial.flush();
		}
		
		private: template <typename T> static size_t _print(T&& param) { return Serial.print(std::forward<T>(param)); }
		private: static size_t _print(Functional& param) { return param(); }
		
		private: static size_t _pass() { return 0; }
		private: template <typename T, typename... Types> static size_t _pass(T&& param, Types&&... params)
		{
			return
				_print(std::forward<T>(param))
				+
				_pass(std::forward<Types>(params)...);
		}
		
		public: template <typename... Types> static size_t print(Types&&... params)
		{
			System::lock();
			size_t s = _pass(std::forward<Types>(params)...);
			System::unlock();
			return s;
		}
		public: template <typename... Types> static size_t println(Types&&... params)
		{
			System::lock();
			size_t s =
				_pass(std::forward<Types>(params)...)
				+
				Serial.println();
			System::unlock();
			return s;
		}
		
		public: template <typename T> static size_t print_n(T&& data, int base = DEC)
		{
			System::lock();
			size_t s = Serial.print(data, base);
			System::unlock();
			return s;
		}
		public: template <typename T> static size_t println_n(T&& data, int base = DEC)
		{
			System::lock();
			size_t s = Serial.println(data, base);
			System::unlock();
			return s;
		}
		
		public: static size_t write(uint8_t data)
		{
			System::lock();
			size_t s = Serial.write(data);
			System::unlock();
			return s;
		}
		public: static size_t write(const char* str)
		{
			System::lock();
			size_t s = Serial.write(str);
			System::unlock();
			return s;
		}
		public: static size_t write(const char* buffer, size_t size)
		{
			System::lock();
			size_t s = Serial.write(buffer, size);
			System::unlock();
			return s;
		}
		public: static size_t write(const uint8_t* buffer, size_t size)
		{
			System::lock();
			size_t s = Serial.write(buffer, size);
			System::unlock();
			return s;
		}
	};

	class SerialOutputStream final : public data::OutputStream
	{
		public: virtual ~SerialOutputStream() { }
		
		public: virtual size_t print(const __FlashStringHelper* data) override { return SerialMonitor::print(data); }
		public: virtual size_t print(const String& data) override { return SerialMonitor::print(data); }
		public: virtual size_t print(const char data[]) override { return SerialMonitor::print(data); }
		public: virtual size_t print(char data) override { return SerialMonitor::print(data); }
		public: virtual size_t print(unsigned char data, int base = DEC) override { return SerialMonitor::print_n(data, base); }
		public: virtual size_t print(int data, int base = DEC) override { return SerialMonitor::print_n(data, base); }
		public: virtual size_t print(unsigned int data, int base = DEC) override { return SerialMonitor::print_n(data, base); }
		public: virtual size_t print(long data, int base = DEC) override { return SerialMonitor::print_n(data, base); }
		public: virtual size_t print(unsigned long data, int base = DEC) override { return SerialMonitor::print_n(data, base); }
		public: virtual size_t print(double data, int base = 2) override { return SerialMonitor::print_n(data, base); }
		
		public: virtual size_t println(const __FlashStringHelper* data) override { return SerialMonitor::println(data); }
		public: virtual size_t println(const String& data) override { return SerialMonitor::println(data); }
		public: virtual size_t println(const char data[]) override { return SerialMonitor::println(data); }
		public: virtual size_t println(char data) override { return SerialMonitor::println(data); }
		public: virtual size_t println(unsigned char data, int base = DEC) override { return SerialMonitor::println_n(data, base); }
		public: virtual size_t println(int data, int base = DEC) override { return SerialMonitor::println_n(data, base); }
		public: virtual size_t println(unsigned int data, int base = DEC) override { return SerialMonitor::println_n(data, base); }
		public: virtual size_t println(long data, int base = DEC) override { return SerialMonitor::println_n(data, base); }
		public: virtual size_t println(unsigned long data, int base = DEC) override { return SerialMonitor::println_n(data, base); }
		public: virtual size_t println(double data, int base = 2) override { return SerialMonitor::println_n(data, base); }
		public: virtual size_t println(void) override { return SerialMonitor::println(); }
		
		public: virtual size_t write(uint8_t data) override { return SerialMonitor::write(data); }
		public: virtual size_t write(const char* str) override { return SerialMonitor::write(str); }
		public: virtual size_t write(const char* buffer, size_t size) override { return SerialMonitor::write(buffer, size); }
		public: virtual size_t write(const uint8_t* buffer, size_t size) override { return SerialMonitor::write(buffer, size); }
	};
}
