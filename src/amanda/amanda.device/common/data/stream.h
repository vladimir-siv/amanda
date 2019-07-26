#pragma once

namespace data
{
	class Stream
	{
		public: virtual ~Stream() { }
		
		public: virtual char current() const = 0;
		public: virtual void next() = 0;
		public: virtual bool reset() { return false; }
		public: virtual bool eos() const { return current() == 0; }
		public: char advance() { char c = current(); next(); return c; }
	};

	class RAMStream final : public Stream
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
