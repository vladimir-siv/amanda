#pragma once

#include <Arduino.h>
#include <system.h>

class Scanner
{
	public: virtual ~Scanner() { }
	
	public: virtual void scan(byte value) = 0;
	public: virtual void scan(int value) = 0;
	public: virtual void scan(char value) = 0;
	public: virtual void scan(unsigned long value) = 0;
	public: virtual void scan(float value) = 0;
	public: virtual void scan(double value) = 0;
	public: virtual void scan(const char* value) = 0;
	public: virtual void scan(const __FlashStringHelper* value) = 0;
};

class SerialScanner : public Scanner
{
	private: template <typename T> void _print(T&& value)
	{
		System::lock();
		Serial.print(value);
		System::unlock();
	}
	
	public: virtual ~SerialScanner() { }
	
	public: virtual void scan(byte value) { _print(value); }
	public: virtual void scan(int value) { _print(value); }
	public: virtual void scan(char value) { _print(value); }
	public: virtual void scan(unsigned long value) { _print(value); }
	public: virtual void scan(float value) { _print(value); }
	public: virtual void scan(double value) { _print(value); }
	public: virtual void scan(const char* value) { _print(value); }
	public: virtual void scan(const __FlashStringHelper* value) { _print(value); }
};
