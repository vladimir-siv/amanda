#pragma once

#include "component.h"

class ISensor : public IComponent
{
	public: virtual ~ISensor() override = 0;
	public: virtual Type ctype() const override { return Type::SENSOR; };
	public: virtual byte pin() const = 0;
};

class DigitalSensor : public ISensor
{
	DigitalSensor(const DigitalSensor&) = delete;
	DigitalSensor(DigitalSensor&&) = delete;
	DigitalSensor& operator=(const DigitalSensor&) = delete;
	DigitalSensor& operator=(DigitalSensor&&) = delete;

	private: static inline CID newcid() { static unsigned long long id = 0; return (CID)++id; }
	private: CID _id = newcid();
	
	protected: byte _pin = 0;
	
	public: explicit DigitalSensor(byte pin) : _pin(pin) { pinMode(pin, INPUT); }
	public: virtual ~DigitalSensor() override = 0;
	
	public: virtual CID ID() const final override { return _id; }
	public: virtual Type ctype() const final override { return (Type)(ISensor::ctype() | Type::DIGITAL); }
	public: virtual byte pin() const final override { return _pin; }
	
	public: virtual DigitalState read() const { return digitalRead(_pin); };
};

class AnalogSensor : public ISensor
{
	AnalogSensor(const AnalogSensor&) = delete;
	AnalogSensor(AnalogSensor&&) = delete;
	AnalogSensor& operator=(const AnalogSensor&) = delete;
	AnalogSensor& operator=(AnalogSensor&&) = delete;

	private: static inline CID newcid() { static unsigned long long id = 0; return (CID)++id; }
	private: CID _id = newcid();
	
	protected: byte _pin = 0;
	
	public: explicit AnalogSensor(byte pin) : _pin(pin) { pinMode(pin, INPUT); }
	public: virtual ~AnalogSensor() override = 0;
	
	public: virtual CID ID() const final override { return _id; }
	public: virtual Type ctype() const final override { return (Type)(ISensor::ctype() | Type::ANALOG); }
	public: virtual byte pin() const final override { return _pin; }
	
	public: virtual AnalogValue read() const { return analogRead(_pin); }
	public: virtual const char* const unit() const { return nullptr; };
};
