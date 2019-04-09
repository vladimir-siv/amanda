#pragma once

#include "../../types/def.h"

template <typename T, size_t _capacity = 3>
class QBuffer final
{
	private: unsigned int _size = 0;
	private: unsigned int _top = 0;
	private: T _data[_capacity];
	
	public: unsigned int capacity() const { return _capacity; }
	public: unsigned int size() const { return _size; }
	
	public: void clear()
	{
		_size = _top = 0;
	}
	
	public: void push(T _value)
	{
		_data[_top] = _value;
		if (++_top == _capacity) _top = 0;
		if (++_size > _capacity) _size = _capacity;
	}
	public: T pop()
	{
		if (_size > 0)
		{
			--_size;
			if (_top-- == 0) _top = _capacity - 1;
		}

		return _data[_top];
	}
	public: T peek() const
	{
		return _data[_top == 0 ? _capacity - 1 : _top - 1];
	}
};
