#pragma once

#include <Arduino.h>
#include "../../../common/units.h"
#include "../../../lib/string64.h"

using VID = const unsigned long;
using DigitalState = int;
using AnalogValue = float;

// struct-like class
class Command final
{
	public: String64 name;
	public: String64 args[5];
	
	public: explicit Command(const char* name = nullptr, const char* arg1 = nullptr, const char* arg2 = nullptr, const char* arg3 = nullptr, const char* arg4 = nullptr, const char* arg5 = nullptr) : name(name), args { arg1, arg2, arg3, arg4, arg5 } {  }
	public: Command(const char* name, const char* args[5]) : Command(name, args[0], args[1], args[2], args[3], args[4]) { }
};

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
	public: static Type resolveType(const char* name)
	{
		if (strcmp_P(name, PSTR("DS")) == 0) return DigitalSensor;
		if (strcmp_P(name, PSTR("AS")) == 0) return AnalogSensor;
		if (strcmp_P(name, PSTR("DE")) == 0) return DigitalElement;
		if (strcmp_P(name, PSTR("AE")) == 0) return AnalogElement;

		return Type::NONE;
	}
	
	public: virtual ~IComponent() = 0;
	
	public: virtual VID ID() const = 0;
	public: virtual Type ctype() const = 0;
	public: virtual const __FlashStringHelper* description() const { return F("component"); }
	
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
	public: const __FlashStringHelper* type_str() const
	{
		if (ctype() == NONE) return F("");

		if (is((Type)1)) return F("D");
		if (is((Type)2)) return F("A");
		if (is((Type)3)) return F("DA");
		if (is((Type)4)) return F("S");
		if (is((Type)5)) return F("DS");
		if (is((Type)6)) return F("AS");
		if (is((Type)7)) return F("DAS");
		if (is((Type)8)) return F("E");
		if (is((Type)9)) return F("DE");
		if (is((Type)10)) return F("AE");
		if (is((Type)11)) return F("DAE");
		if (is((Type)12)) return F("SE");
		if (is((Type)13)) return F("DSE");
		if (is((Type)14)) return F("ASE");
		if (is((Type)15)) return F("DASE");

		return F("");
	}
	
	public: virtual const __FlashStringHelper* commands() const { return F("||"); }
	public: virtual CommandResult execute(const Command& command) { return CommandResult::null(); };
};
