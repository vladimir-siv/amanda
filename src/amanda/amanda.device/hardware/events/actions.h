#pragma once

#include <Arduino.h>

#include <dependency.h>
#include <extensions/memory_management/sdd_list.h>

#include "../../common/data/stream.h"

#include "../components.h"

class action final
{
	private: sdd::type<IElement*> element;
	private: sdd::type<float> value;
	
	public: static action* _new(IElement* element, float value)
	{
		return D::sdds->alloc<action>(sdd::cast(element), sdd::cast(value));
	}
	public: action(IElement* element, float value) :
		element(element),
		value(value)
	{ }
	
	public: IElement* getElement() const { return element.real; }
	public: void setElement(IElement* element) { this->element.real = element; }
	public: float getValue() const { return value.real; }
	public: void setValue(float value) { this->value.real = value; }
	
	public: void execute() const
	{
		DigitalElement* del = ComponentCaster::digital_element(element.real);
		if (del != nullptr)
		{
			del->write((DigitalState)value.real);
			return;
		}

		AnalogElement* ael = ComponentCaster::analog_element(element.real);
		if (ael != nullptr)
		{
			ael->write((AnalogValue)value.real);
			return;
		}
	}
	
	public: void to_xml(data::OutputStream& stream)
	{
		stream.print(F("<write vid=\""));
		stream.print(element.real->ID());
		stream.print(F("\" ctype=\""));
		stream.print(element.real->type_str());
		stream.print(F("\">"));

		DigitalElement* del = ComponentCaster::digital_element(element.real);
		if (del != nullptr)
		{
			stream.print(F("<state>"));
			stream.print((long)value.real);
			stream.print(F("</state>"));
			goto done;
		}

		AnalogElement* ael = ComponentCaster::analog_element(element.real);
		if (ael != nullptr)
		{
			stream.print(F("<value unit=\"V\">"));
			stream.print(value.real);
			stream.print(F("</value>"));
			goto done;
		}

	done:
		stream.print(F("</write>"));
	}
};

class actions final
{
	private: sdd::type<sdd_list<action>*> _raise;
	private: sdd::type<sdd_list<action>*> _expire;
	
	public: static actions* _new()
	{
		auto _raise = sdd_list<action>::_new();
		auto _expire = sdd_list<action>::_new();
		return D::sdds->alloc<actions>(sdd::cast(_raise), sdd::cast(_expire));
	}
	public: actions() :
		_raise(sdd_list<action>::_new()),
		_expire(sdd_list<action>::_new())
	{ }
	public: ~actions()
	{
		auto _raise = sdd_assume::list(this->_raise.real);
		auto _expire = sdd_assume::list(this->_expire.real);

		for (auto i = _raise.begin(); i != _raise.end(); ++i) D::sdds->dealloc(*i);
		for (auto i = _expire.begin(); i != _expire.end(); ++i) D::sdds->dealloc(*i);

		_raise.clear();
		_expire.clear();

		//D::vlists->dealloc(_raise.real);
		//D::vlists->dealloc(_expire.real);

		this->_raise.real = nullptr;
		this->_expire.real = nullptr;
	}
	
	public: void appendRaise(action* act)
	{
		auto _raise = sdd_assume::list(this->_raise.real);
		_raise.push(act);
	}
	public: void appendExpire(action* act)
	{
		auto _expire = sdd_assume::list(this->_expire.real);
		_expire.push(act);
	}
	
	public: void raise() const
	{
		auto _raise = sdd_assume::list(this->_raise.real);

		for (auto i = _raise.cbegin(); i != _raise.cend(); ++i)
		{
			i->execute();
		}
	}
	public: void expire() const
	{
		auto _expire = sdd_assume::list(this->_expire.real);
		
		for (auto i = _expire.cbegin(); i != _expire.cend(); ++i)
		{
			i->execute();
		}
	}
	
	public: void to_xml(data::OutputStream& stream)
	{
		auto _raise = sdd_assume::list(this->_raise.real);
		auto _expire = sdd_assume::list(this->_expire.real);

		stream.print(F("<actions>"));

		stream.print(F("<raise>"));
		for (auto i = _raise.begin(); i != _raise.end(); ++i)
		{
			i->to_xml(stream);
		}
		stream.print(F("</raise>"));

		stream.print(F("<expire>"));
		for (auto i = _expire.begin(); i != _expire.end(); ++i)
		{
			i->to_xml(stream);
		}
		stream.print(F("</expire>"));

		stream.print(F("</actions>"));
	}
};
