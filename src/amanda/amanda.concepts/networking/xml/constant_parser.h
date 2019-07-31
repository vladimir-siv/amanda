#pragma once

class ConstantParser final
{
	public: class CStream
	{
		public: virtual ~CStream() { }
		
		public: char advance() { char c = current(); next(); return c; }
		public: virtual char current() const = 0;
		public: virtual void next() = 0;
		public: virtual bool eos() const = 0;
		public: virtual bool reset() = 0;
	};
	
	public: enum Result { ERROR, NEXT, DONE };
	
	private: CStream* _constant = nullptr;
	private: Result _last = DONE;
	
	public: ConstantParser() { }
	public: ConstantParser(CStream& constant) { parse(constant); }
	
	public: bool parse(CStream& constant)
	{
		if (_last != DONE) return false;
		_constant = &constant;
		_last = NEXT;
		return true;
	}
	public: bool reset(bool hard = true)
	{
		if (hard)
		{
			_constant = nullptr;
			_last = DONE;
			return true;
		}
		else
		{
			_last = NEXT;
			return _constant->reset();
		}
	}
	public: Result next(char c)
	{
		if (_last == NEXT && _constant != nullptr && !_constant->eos())
		{
			if (_constant->advance() == c)
			{
				if (_constant->eos()) return _last = DONE;
				else return _last = NEXT;
			}
		}
		
		return _last = ERROR;
	}
};
