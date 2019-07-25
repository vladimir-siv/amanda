#pragma once

#include "stream.h"

#include <Arduino.h>

namespace xml
{
	class FlashStream final : public Stream
	{
		private: PGM_P _xml;
		private: PGM_P _current;
		private: char c;

		public: FlashStream(const __FlashStringHelper* xml) :
			_xml(reinterpret_cast<PGM_P>(xml)),
			_current(reinterpret_cast<PGM_P>(xml))
		{
			read_current();
		}

		private: void read_current()
		{
			c = pgm_read_byte(_current);
		}

		public: virtual char current() const override
		{
			return c;
		}
		public: virtual void next() override
		{
			++_current;
			read_current();
		}
		public: virtual bool reset() override
		{
			_current = _xml;
			read_current();
			return true;
		}
	};
}
