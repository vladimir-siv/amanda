#pragma once

#include <Arduino.h>

using CID = const unsigned long long;
using DigitalState = int;
using AnalogValue = int;

class CommandResult
{
	protected: static CommandResult _null_result;
	public: static const CommandResult& null() { return _null_result; }
	
	protected: int _result;
	protected: bool _is_null;
	
	public: CommandResult() : CommandResult(0, true) { }
	public: explicit CommandResult(int result, bool is_null = false) : _result(result), _is_null(is_null) { }
	public: virtual ~CommandResult() { }
	
	public: virtual DigitalState as_digital() const { return _result; }
	public: virtual AnalogValue as_analog() const { return _result; }
	public: virtual bool is_null() const { return _is_null; }
	public: virtual bool equals(const CommandResult& other) const { return (_is_null == true && other._is_null == true) || (_is_null == other._is_null && _result == other._result); }
	public: friend bool operator==(const CommandResult& r1, const CommandResult& r2) { return r1.equals(r2); }
	public: friend bool operator!=(const CommandResult& r1, const CommandResult& r2) { return !(r1 == r2); }
};

class IComponent
{
	public: enum Type { NONE = 0, DIGITAL = 1, ANALOG = 2, SENSOR = 4, ELEMENT = 8 };
	public: virtual ~IComponent() = 0;
	
	public: virtual CID ID() const = 0;
	public: virtual Type ctype() const = 0;
	
	public: bool is(Type type) const
	{
		Type this_type = ctype();
		Type ty = (Type)(this_type ^ type);
		return this_type != Type::NONE && ty == Type::NONE;
	}
	public: bool is_any(Type type) const
	{
		Type this_type = ctype();
		Type ty = (Type)(this_type & type);
		return this_type != Type::NONE && ty != Type::NONE;
	}
	
	public: virtual String commands() const { return "||"; }
	public: virtual CommandResult execute(const String& command) { return CommandResult::null(); };
};
