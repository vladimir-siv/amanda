#pragma once

namespace xml
{
	class Stream
	{
		public: virtual ~Stream() { }
		
		public: virtual char current() const = 0;
		public: virtual void next() = 0;
		public: virtual bool reset() { return false; }
		public: char advance() { char c = current(); next(); return c; }
		public: bool eos() const { return current() == 0; }
	};

	class RAMStream final : public Stream
	{
		private: const char* _xml;
		private: const char* _current;
		
		public: RAMStream(const char* xml) : _xml(xml), _current(xml) { }
		
		public: virtual char current() const override
		{
			return *_current;
		}
		public: virtual void next() override
		{
			++_current;
		}
		public: virtual bool reset() override
		{
			_current = _xml;
			return true;
		}
	};
}
