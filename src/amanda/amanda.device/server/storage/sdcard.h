#pragma once

#include <Arduino.h>
#include <SPI/src/SPI.h>
#include <SD/src/SD.h>

#include "../../common/data/stream.h"

namespace storage
{
	class FileStream final : public data::Stream
	{
		private: File _file;
		private: char c;
		
		public: FileStream(const __FlashStringHelper* file) : _file(SD.open(file)) { next(); }
		public: FileStream(const char* file) : _file(SD.open(file)) { next(); }
		public: virtual ~FileStream() { _file.close(); }
		
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
		
		public: static bool WriteToFile(File& file, data::Stream& content)
		{
			if (content.eos()) return false;

			while (!content.eos())
			{
				file.print(content.advance());
			}

			return true;
		}
		public: static bool WriteToFile(File& file, data::Stream&& content) { return WriteToFile(file, content); }
		public: static bool WriteToFile(File&& file, data::Stream& content) { return WriteToFile(file, content); }
		public: static bool WriteToFile(File&& file, data::Stream&& content) { return WriteToFile(file, content); }
		public: template <typename T> static bool WriteToFile(T&& file, data::Stream& content)
		{
			File f = SD.open(file, FILE_WRITE);
			bool succ = WriteToFile(f, content);
			f.close();
			return succ;
		}
		public: template <typename T> static bool WriteToFile(T&& file, data::Stream&& content) { return WriteToFile(file, content); }
		public: template <typename T> static bool WriteToFile(T&& file, const char* content) { return WriteToFile(file, data::RAMStream(content)); }
		
		public: static bool PrintFileToSerial(FileStream& file)
		{
			while (!file.eos()) Serial.print(file.advance());
			return true; // ?
		}
		public: static bool PrintFileToSerial(FileStream&& file) { return PrintFileToSerial(file); }
		public: template <typename T> static bool PrintFileToSerial(T&& file) { return PrintFileToSerial(FileStream(file)); }
	};
}
