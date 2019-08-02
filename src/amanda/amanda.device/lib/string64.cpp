#include "string64.h"

unsigned long String64::parse(const char* str, unsigned int base = DEC)
{
	static auto val = [](char chr) -> unsigned long
	{
		if ('0' <= chr && chr <= '9') return (unsigned long)(chr - '0');
		if ('A' <= chr && chr <= 'Z') return (unsigned long)(chr - 'A') + 10ul;
		if ('a' <= chr && chr <= 'z') return (unsigned long)(chr - 'a') + 36ul;
		return 0;
	};

	if (str == nullptr || strcmp_P(str, PSTR("")) == 0) return 0;

	if (base < 2 || base >= 62) return 0;

	unsigned long value = 0;

	for (; *str != 0; ++str)
	{
		value = value * base + val(*str);
	}

	return value;
}

unsigned int String64::index_of(char c)
{
	char* current = _str;
	while (*current != 0 && *current != c) ++current;
	return current - _str;
}

void String64::cut(unsigned int index)
{
	if (index >= _length) return;
	_str[_length = index] = 0;
}

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

bool String64::equals(const __FlashStringHelper* str) const
{
	PGM_P _current = reinterpret_cast<PGM_P>(str);
	char c = 0;
	auto advance = [&]() -> char
	{
		c = pgm_read_byte(_current);
		if (c != 0) ++_current;
		return c;
	};

	if (_str[_length] != 0) return false;

	for (unsigned int i = 0; i < _length; ++i)
	{
		if (_str[i] != advance()) return false;
	}

	if (advance() != 0) return false;

	return true;
}

String64& String64::operator=(const char* str)
{
	clear();
	return operator+=(str);
}

String64& String64::operator+=(const String64& str)
{
	return operator+=(str.c_str());
}

String64& String64::operator+=(const char* str)
{
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

String64& String64::operator+=(const __FlashStringHelper* str)
{
	PGM_P _current = reinterpret_cast<PGM_P>(str);
	char c = pgm_read_byte(_current);
	auto advance = [&]() -> char
	{
		char t = c;
		if (c != 0) c = pgm_read_byte(++_current);
		return t;
	};

	if (str != nullptr)
	{
		while (c != 0 && _length < 64)
		{
			_str[_length++] = advance();
		}
	}

	_str[_length] = 0;

	return *this;
}

String64& String64::operator+=(char c)
{
	if (_length < 64)
	{
		_str[_length++] = c;
		_str[_length] = 0;
	}

	return *this;
}

String64& String64::append(unsigned long val, unsigned int base = DEC)
{
	static auto chr = [](unsigned long val) -> char
	{
		if (val < 10) return '0' + val;
		if (val < 36) return 'A' + (val - 10);
		if (val < 62) return 'a' + (val - 36);
		return 0;
	};

	if (base < 2 || base >= 62) return *this;

	if (val == 0)
	{
		operator+=('0');
		return *this;
	}

	char digits[64];
	byte len = 0;

	while (val > 0)
	{
		digits[len++] = chr(val % base);
		val /= base;
	}

	while (len > 0)
	{
		operator+=(digits[--len]);
	}

	return *this;
}
