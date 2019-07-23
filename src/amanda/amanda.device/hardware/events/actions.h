#pragma once

#include <Arduino.h>

#include <dependency.h>

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
};

class actions final
{
	private: sdd::type<vlist<action>*> _raise;
	private: sdd::type<vlist<action>*> _expire;
	
	public: static actions* _new()
	{
		auto _raise = D::vlists->alloc<action>(D::nodes);
		auto _expire = D::vlists->alloc<action>(D::nodes);
		return D::sdds->alloc<actions>(sdd::cast(_raise), sdd::cast(_expire));
	}
	public: actions() :
		_raise(D::vlists->alloc<action>(D::nodes)),
		_expire(D::vlists->alloc<action>(D::nodes))
	{ }
	public: ~actions()
	{
		for (auto i = _raise.real->begin(); i != _raise.real->end(); ++i) D::sdds->dealloc(*i);
		for (auto i = _expire.real->begin(); i != _expire.real->end(); ++i) D::sdds->dealloc(*i);

		_raise.real->clear();
		_expire.real->clear();

		D::vlists->dealloc(_raise.real);
		D::vlists->dealloc(_expire.real);

		_raise.real = nullptr;
		_expire.real = nullptr;
	}
	
	public: void appendRaise(action* act)
	{
		if (_raise.real) _raise.real->push_back(act);
	}
	public: void appendExpire(action* act)
	{
		if (_expire.real) _expire.real->push_back(act);
	}
	
	public: void raise() const
	{
		if (_raise.real)
		{
			for (auto i = _raise.real->cbegin(); i != _raise.real->cend(); ++i)
			{
				i->execute();
			}
		}
	}
	public: void expire() const
	{
		if (_expire.real)
		{
			for (auto i = _expire.real->cbegin(); i != _expire.real->cend(); ++i)
			{
				i->execute();
			}
		}
	}
};
