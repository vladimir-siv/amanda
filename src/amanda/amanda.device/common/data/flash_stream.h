#pragma once

#include "stream.h"

#include <Arduino.h>

namespace data
{
	class FlashStream final : public InputStream
	{
		private: PGM_P _content;
		private: PGM_P _current;
		private: char c;

		public: FlashStream(const __FlashStringHelper* content) :
			_content(reinterpret_cast<PGM_P>(content)),
			_current(reinterpret_cast<PGM_P>(content)),
			c('?')
		{ next(); }
		
		public: virtual char current() const override
		{
			return c;
		}
		public: virtual void next() override
		{
			if (!eos()) c = pgm_read_byte(_current++);
		}
		public: virtual bool reset() override
		{
			_current = _content;
			c = '?';
			next();
			return true;
		}
	};
}
