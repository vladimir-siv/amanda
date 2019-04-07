#pragma once

#include <Arduino.h>

using VID = const unsigned long;
using DigitalState = int;
using AnalogValue = float;

// struct-like class
class CommandResult
{
	protected: static CommandResult _null_result;
	public: static const CommandResult& null() { return _null_result; }
	
	protected: union Result final
	{
		DigitalState digital;
		AnalogValue analog;
		Result(DigitalState digital) : digital(digital) { }
		Result(AnalogValue analog) : analog(analog) { }
		bool equals(const Result& other) const { return digital == other.digital || analog == other.analog; }
	} _result;
	protected: bool _is_null;
	
	public: CommandResult() : CommandResult(0, true) { }
	public: explicit CommandResult(DigitalState digitalValue, bool is_null = false) : _result(digitalValue), _is_null(is_null) { }
	public: explicit CommandResult(AnalogValue analogValue, bool is_null = false) : _result(analogValue), _is_null(is_null) { }
	public: virtual ~CommandResult() { }
	
	public: virtual DigitalState as_digital() const { return _result.digital; }
	public: virtual AnalogValue as_analog() const { return _result.analog; }
	public: virtual bool is_null() const { return _is_null; }
	public: virtual bool equals(const CommandResult& other) const { return (_is_null == true && other._is_null == true) || (_is_null == other._is_null && _result.equals(other._result)); }
	public: friend bool operator==(const CommandResult& r1, const CommandResult& r2) { return r1.equals(r2); }
	public: friend bool operator!=(const CommandResult& r1, const CommandResult& r2) { return !(r1 == r2); }
};

class IComponent
{
	public: enum Type { NONE = 0, DIGITAL = 1, ANALOG = 2, SENSOR = 4, ELEMENT = 8 };
	
	public: static const Type DigitalSensor = (Type)(DIGITAL | SENSOR);
	public: static const Type AnalogSensor = (Type)(ANALOG | SENSOR);
	public: static const Type DigitalElement = (Type)(DIGITAL | ELEMENT);
	public: static const Type AnalogElement = (Type)(ANALOG | ELEMENT);
	
	public: virtual ~IComponent() = 0;
	
	public: virtual VID ID() const = 0;
	public: virtual Type ctype() const = 0;
	public: virtual const char* description() const { return "component"; }
	
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
	public: const char* type_str() const
	{
		if (ctype() == NONE) return "";

		if (is((Type)1)) return "D";
		if (is((Type)2)) return "A";
		if (is((Type)3)) return "DA";
		if (is((Type)4)) return "S";
		if (is((Type)5)) return "DS";
		if (is((Type)6)) return "AS";
		if (is((Type)7)) return "DAS";
		if (is((Type)8)) return "E";
		if (is((Type)9)) return "DE";
		if (is((Type)10)) return "AE";
		if (is((Type)11)) return "DAE";
		if (is((Type)12)) return "SE";
		if (is((Type)13)) return "DSE";
		if (is((Type)14)) return "ASE";
		if (is((Type)15)) return "DASE";

		return "";
	}
	
	public: virtual const char* commands() const { return "||"; }
	public: virtual CommandResult execute(const String& command) { return CommandResult::null(); };
};
