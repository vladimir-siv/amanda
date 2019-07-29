#pragma once

#include <Arduino.h>
#include <SPI/src/SPI.h>
#include <SD/src/SD.h>

#include "../../common/data/stream.h"

namespace storage
{
	class InputFileStream : public data::InputStream
	{
		private: File _file;
		private: char c;
		
		public: InputFileStream(const __FlashStringHelper* file) : _file(SD.open(file)) { next(); }
		public: InputFileStream(const char* file) : _file(SD.open(file)) { next(); }
		public: virtual ~InputFileStream() { _file.close(); }
		
		public: virtual char current() const override
		{
			return c;
		}
		public: virtual void next() override
		{
			if (!eos()) c = (char)_file.read();
		}
		public: virtual bool reset() override
		{
			return _file.seek(0);
		}
		public: virtual bool eos() const override
		{
			return const_cast<File&>(_file).available() == 0;
		}
	};

	class OutputFileStream : public data::OutputStream
	{
		private: File _file;
		
		public: OutputFileStream(const __FlashStringHelper* file) : _file(SD.open(file, FILE_WRITE)) { }
		public: OutputFileStream(const char* file) : _file(SD.open(file, FILE_WRITE)) { }
		public: virtual ~OutputFileStream() { _file.close(); }
		
		public: virtual size_t print(const __FlashStringHelper* data) override { return _file.print(data); }
		public: virtual size_t print(const String& data) override { return _file.print(data); }
		public: virtual size_t print(const char data[]) override { return _file.print(data); }
		public: virtual size_t print(char data) override { return _file.print(data); }
		public: virtual size_t print(unsigned char data, int base = DEC) override { return _file.print(data, base); }
		public: virtual size_t print(int data, int base = DEC) override { return _file.print(data, base); }
		public: virtual size_t print(unsigned int data, int base = DEC) override { return _file.print(data, base); }
		public: virtual size_t print(long data, int base = DEC) override { return _file.print(data, base); }
		public: virtual size_t print(unsigned long data, int base = DEC) override { return _file.print(data, base); }
		public: virtual size_t print(double data, int base = 2) override { return _file.print(data, base); }
		public: virtual size_t print(const Printable& data) override { return _file.print(data); }
		
		public: virtual size_t println(const __FlashStringHelper* data) override { return _file.println(data); }
		public: virtual size_t println(const String& data) override { return _file.println(data); }
		public: virtual size_t println(const char data[]) override { return _file.println(data); }
		public: virtual size_t println(char data) override { return _file.println(data); }
		public: virtual size_t println(unsigned char data, int base = DEC) override { return _file.println(data, base); }
		public: virtual size_t println(int data, int base = DEC) override { return _file.println(data, base); }
		public: virtual size_t println(unsigned int data, int base = DEC) override { return _file.println(data, base); }
		public: virtual size_t println(long data, int base = DEC) override { return _file.println(data, base); }
		public: virtual size_t println(unsigned long data, int base = DEC) override { return _file.println(data, base); }
		public: virtual size_t println(double data, int base = 2) override { return _file.println(data, base); }
		public: virtual size_t println(const Printable& data) override { return _file.println(data); }
		public: virtual size_t println(void) override { return _file.println(); }
		
		public: virtual size_t write(uint8_t data) override { return _file.write(data); }
		public: virtual size_t write(const char* str) override { return _file.write(str); }
		public: virtual size_t write(const char* buffer, size_t size) override { return _file.write(buffer, size); }
		public: virtual size_t write(const uint8_t* buffer, size_t size) override { return _file.write(buffer, size); }
	};

	class SDCard final
	{
		private: SDCard() { }
		
		public: static bool init(uint8_t _cs_pin = 4)
		{
			static bool initialized = false;
			static bool success = false;

			if (!initialized)
			{
				pinMode(_cs_pin, OUTPUT);
				success = SD.begin(_cs_pin);
				initialized = true;
			}

			return success;
		}
		
		public: static bool WriteToFile(File& file, data::InputStream& content)
		{
			if (content.eos()) return false;

			while (!content.eos())
			{
				file.print(content.advance());
			}

			return true;
		}
		public: static bool WriteToFile(File& file, data::InputStream&& content) { return WriteToFile(file, content); }
		public: static bool WriteToFile(File&& file, data::InputStream& content) { return WriteToFile(file, content); }
		public: static bool WriteToFile(File&& file, data::InputStream&& content) { return WriteToFile(file, content); }
		public: template <typename T> static bool WriteToFile(T&& file, data::InputStream& content)
		{
			File f = SD.open(file, FILE_WRITE);
			bool succ = WriteToFile(f, content);
			f.close();
			return succ;
		}
		public: template <typename T> static bool WriteToFile(T&& file, data::InputStream&& content) { return WriteToFile(file, content); }
		public: template <typename T> static bool WriteToFile(T&& file, const char* content) { return WriteToFile(file, data::RAMStream(content)); }
		
		public: static bool PrintFileToSerial(InputFileStream& file)
		{
			while (!file.eos()) Serial.print(file.advance());
			return true; // ?
		}
		public: static bool PrintFileToSerial(InputFileStream&& file) { return PrintFileToSerial(file); }
		public: template <typename T> static bool PrintFileToSerial(T&& file) { return PrintFileToSerial(InputFileStream(file)); }
	};
}
