#pragma once

#include <Arduino.h>
#include "../../../common/units.h"
#include "../../../lib/string64.h"

using VID = const unsigned long;
using DigitalState = int;
using AnalogValue = float;

class IComponent;
namespace ethernet { class HTTPClient; }

class Command
{
	friend class IComponent;

	protected: IComponent* component = nullptr;
	
	public: virtual ~Command() { }
	
	public: IComponent* get_component() const { return component; }
	public: virtual void configure() = 0;
	public: virtual void accept(unsigned long argn, const char* argv) = 0;
	public: virtual bool execute() = 0;
	public: virtual bool respond(ethernet::HTTPClient& client, bool succ) const { return false; }
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
	
	public: virtual ~IComponent() { }
	
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
	public: virtual Command* resolve_cmd(const char* name) const { return nullptr; }
	protected: void bind_to(Command* cmd) const { if (cmd != nullptr) cmd->component = const_cast<IComponent*>(this); }
};
