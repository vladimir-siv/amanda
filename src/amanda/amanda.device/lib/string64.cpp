#include "string64.h"

bool String64::equals(const String64& other) const
{
	if (_length != other._length) return false;
	if (_str[_length] != 0) return false;
	if (other._str[other._length] != 0) return false;

	for (unsigned int i = 0; i < _length; ++i)
	{
		if (_str[i] != other._str[i]) return false;
	}

	return true;
}

bool String64::equals(const char* str) const
{
	if (_str[_length] != 0) return false;

	for (unsigned int i = 0; i < _length; ++i)
	{
		if (_str[i] != *(str++)) return false;
	}

	if (*str != 0) return false;

	return true;
}

String64& String64::operator=(const char* str)
{
	_length = 0;
	_str[64] = 0;

	if (str != nullptr)
	{
		while (*str != 0 && _length < 64)
		{
			_str[_length++] = *(str++);
		}
	}

	_str[_length] = 0;

	return *this;
}
