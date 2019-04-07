#pragma once

#include "component.h"

class ISensor : public IComponent
{
	public: virtual ~ISensor() override = 0;
	public: virtual byte pin() const = 0;
	public: virtual Type ctype() const override { return Type::SENSOR; };
	public: virtual const char* description() const override { return "generic sensor"; }
};

class DigitalSensor : public ISensor
{
	DigitalSensor(const DigitalSensor&) = delete;
	DigitalSensor(DigitalSensor&&) = delete;
	DigitalSensor& operator=(const DigitalSensor&) = delete;
	DigitalSensor& operator=(DigitalSensor&&) = delete;

	private: static inline VID newcid() { static unsigned long id = 0; return (VID)++id; }
	private: VID _id = newcid();
	
	protected: byte _pin = 0;
	
	public: explicit DigitalSensor(byte pin) : _pin(pin) { pinMode(pin, INPUT); }
	public: virtual ~DigitalSensor() override = 0;
	
	public: virtual VID ID() const final override { return _id; }
	public: virtual byte pin() const final override { return _pin; }
	public: virtual Type ctype() const override { return (Type)(ISensor::ctype() | Type::DIGITAL); }
	public: virtual const char* description() const override { return "digital sensor"; }
	
	public: virtual DigitalState read() const { return digitalRead(_pin); };
};

class AnalogSensor : public ISensor
{
	AnalogSensor(const AnalogSensor&) = delete;
	AnalogSensor(AnalogSensor&&) = delete;
	AnalogSensor& operator=(const AnalogSensor&) = delete;
	AnalogSensor& operator=(AnalogSensor&&) = delete;

	private: static inline VID newcid() { static unsigned long id = 0; return (VID)++id; }
	private: VID _id = newcid();
	
	protected: byte _pin = 0;
	
	public: explicit AnalogSensor(byte pin) : _pin(pin) { pinMode(pin, INPUT); }
	public: virtual ~AnalogSensor() override = 0;
	
	public: virtual VID ID() const final override { return _id; }
	public: virtual byte pin() const final override { return _pin; }
	public: virtual Type ctype() const override { return (Type)(ISensor::ctype() | Type::ANALOG); }
	public: virtual const char* description() const override { return "analog sensor"; }
	
	public: virtual AnalogValue read() const { return (AnalogValue)analogRead(_pin); }
	public: virtual const char* const unit() const { return nullptr; };
};
