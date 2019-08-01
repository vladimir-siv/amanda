#pragma once

#include <Arduino.h>

class String64
{
	protected: char _str[65] = { };
	protected: unsigned int _length = 0;
	
	public: String64(const char* str = "") { operator=(str); }
	
	public: char& operator[](unsigned int i) { return _str[i]; }
	public: char operator[](unsigned int i) const { return _str[i]; }
	
	public: const char* c_str() const { return _str; }
	public: unsigned int length() const { return _length; }
	public: bool empty() const { return _length == 0; }
	public: String64& clear() { _str[_length = 0] = 0; return *this; }
	
	public: unsigned int index_of(char c);
	public: void cut(unsigned int index);
	
	public: virtual bool equals(const String64& other) const;
	public: virtual bool equals(const char* str) const;
	public: virtual bool equals(const __FlashStringHelper* str) const;
	
	public: String64& operator=(const char* str);
	public: String64& operator+=(const String64& str);
	public: String64& operator+=(const char* str);
	public: String64& operator+=(char c);
	
	public: friend bool operator==(const String64& str1, const String64& str2) { return str1.equals(str2); }
	public: friend bool operator!=(const String64& str1, const String64& str2) { return !(str1 == str2); }
	public: friend bool operator==(const String64& str1, const char* str2) { return str1.equals(str2); }
	public: friend bool operator!=(const String64& str1, const char* str2) { return !(str1 == str2); }
	public: friend bool operator==(const String64& str1, const __FlashStringHelper* str2) { return str1.equals(str2); }
	public: friend bool operator!=(const String64& str1, const __FlashStringHelper* str2) { return !(str1 == str2); }
	//public: bool operator==(const char* str) const { return equals(str); }
	//public: bool operator!=(const char* str) const { return !operator==(str); }
};
