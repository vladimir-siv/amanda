#pragma once

class ConstantParser final
{
	public: enum Result { ERROR, NEXT, DONE };
	
	private: const char* _constant = nullptr;
	private: unsigned int _current = 0;
	private: Result _last = DONE;
	
	public: ConstantParser() { }
	public: ConstantParser(const char* constant) { parse(constant); }
	
	public: bool parse(const char* constant)
	{
		if (_last != DONE) return false;
		_constant = constant;
		_current = 0;
		return true;
	}
	public: void reset()
	{
		_constant = nullptr;
		_current = 0;
		_last = DONE;
	}
	public: Result next(char c)
	{
		if (_constant == nullptr) return _last = ERROR;

		if (_constant[_current] == c)
		{
			++_current;
			if (_constant[_current] == 0) return _last = DONE;
			else return _last = NEXT;
		}
		else return _last = ERROR;
	}
};
