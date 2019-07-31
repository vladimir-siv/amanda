#pragma once

#include "component.h"

class IElement : public IComponent
{
	public: virtual ~IElement() override = 0;
	public: virtual byte pin() const = 0;
	public: virtual Type ctype() const override { return Type::ELEMENT; };
	public: virtual const __FlashStringHelper* description() const override { return F("generic element"); }
};

class DigitalElement : public IElement
{
	DigitalElement(const DigitalElement&) = delete;
	DigitalElement(DigitalElement&&) = delete;
	DigitalElement& operator=(const DigitalElement&) = delete;
	DigitalElement& operator=(DigitalElement&&) = delete;

	private: static inline VID newcid() { static unsigned long id = 0; return (VID)++id; }
	private: VID _id = newcid();
	
	protected: byte _pin = 0;
	
	public: explicit DigitalElement(byte pin) : _pin(pin) { pinMode(pin, OUTPUT); }
	public: virtual ~DigitalElement() override = 0;
	
	public: virtual VID ID() const final override { return _id; }
	public: virtual byte pin() const final override { return _pin; }
	public: virtual Type ctype() const override { return (Type)(IElement::ctype() | Type::DIGITAL); }
	public: virtual const __FlashStringHelper* description() const override { return F("digital element"); }
	
	public: virtual void write(DigitalState state) const { digitalWrite(_pin, state); };
};

class AnalogElement : public IElement
{
	AnalogElement(const AnalogElement&) = delete;
	AnalogElement(AnalogElement&&) = delete;
	AnalogElement& operator=(const AnalogElement&) = delete;
	AnalogElement& operator=(AnalogElement&&) = delete;

	private: static inline VID newcid() { static unsigned long id = 0; return (VID)++id; }
	private: VID _id = newcid();
	
	protected: byte _pin = 0;
	
	public: explicit AnalogElement(byte pin) : _pin(pin) { pinMode(pin, OUTPUT); }
	public: virtual ~AnalogElement() override = 0;
	
	public: virtual VID ID() const final override { return _id; }
	public: virtual byte pin() const final override { return _pin; }
	public: virtual Type ctype() const override { return (Type)(IElement::ctype() | Type::ANALOG); }
	public: virtual const __FlashStringHelper* description() const override { return F("analog element"); }
	
	public: virtual void write(AnalogValue value) const { analogWrite(_pin, value); }
	public: virtual unit e_unit() const { unit u; u.to_default(); return u; }
};
