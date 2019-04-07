#pragma once

#include <Arduino.h>
#include <structures/vector.h>

template <byte _capacity = 32>
class StringBuilder final
{
	private: union sbtype
	{
		const char* string;
		unsigned long number;
		float value;
	};
	
	private: sbtype _content[_capacity];
	private: byte _types[_capacity];
	private: byte _map[_capacity];
	private: byte _top = 0;
	
	public: StringBuilder()
	{
		for (byte i = 0; i < _capacity; ++i)
		{
			_types[i] = 0;
		}
	}
	
	public: byte capacity() const { return _capacity; }
	public: byte size() const { return _top; }
	
	public: bool map(byte i, byte value)
	{
		if (i >= _capacity) return false;
		_map[i] = value;
		return true;
	}
	public: byte map(byte i) const
	{
		if (i >= _capacity) return 0;
		return _map[i];
	}
	
	public: bool set(byte i, const char* string)
	{
		if (i >= _capacity) return false;
		_content[i].string = string;
		_types[i] = 1;
		return true;
	}
	public: bool set(byte i, unsigned long number)
	{
		if (i >= _capacity) return false;
		_content[i].number = number;
		_types[i] = 2;
		return true;
	}
	public: bool set(byte i, float value)
	{
		if (i >= _capacity) return false;
		_content[i].value = value;
		_types[i] = 3;
		return true;
	}
	
	public: bool append(const char* string)
	{
		if (_top >= _capacity) return false;
		_content[_top].string = string;
		_types[_top] = 1;
		++_top;
		return true;
	}
	public: bool append(unsigned long number)
	{
		if (_top >= _capacity) return false;
		_content[_top].number = number;
		_types[_top] = 2;
		++_top;
		return true;
	}
	public: bool append(float value)
	{
		if (_top >= _capacity) return false;
		_content[_top].value = value;
		_types[_top] = 3;
		++_top;
		return true;
	}
	
	public: String to_string() const
	{
		String str;
		
		for (byte i = 0; i < _top; ++i)
		{
			switch (_types[i])
			{
				case 1: str += _content[i].string; break;
				case 2: str += _content[i].number; break;
				case 3: str += _content[i].value; break;
			}
		}

		return str;
	}
	public: void print_to_serial() const
	{
		for (byte i = 0; i < _top; ++i)
		{
			switch (_types[i])
			{
				case 1: Serial.print(_content[i].string); break;
				case 2: Serial.print(_content[i].number); break;
				case 3: Serial.print(_content[i].value); break;
			}
		}
	}
};
