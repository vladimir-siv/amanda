#pragma once

#include <Arduino.h>

#include <dependency.h>
#include <extensions/memory_management/sdd_list.h>

#include "../../common/data/stream.h"

#include "../components.h"

#include "comparator.h"

class cond final
{
	private: sdd::type<ISensor*> sensor;
	private: sdd::type<sdd_list<comparator>*> comps;
	
	public: static cond* _new(ISensor* sensor)
	{
		auto comps = sdd_list<comparator>::_new();
		return D::sdds->alloc<cond>(sdd::cast(sensor), sdd::cast(comps));
	}
	public: cond(ISensor* sensor) :
		sensor(sensor),
		comps(sdd_list<comparator>::_new())
	{ }
	public: ~cond()
	{
		auto comps = sdd_assume::list(this->comps.real);

		for (auto i = comps.begin(); i != comps.end(); ++i) D::sdds->dealloc(*i);
		
		comps.clear();
		//D::vlists->dealloc(comps.real);
		this->comps.real = nullptr;

		sensor.real = nullptr;
	}
	
	public: cond* compare(comparator::cmp cmp, float ref)
	{
		auto comps = sdd_assume::list(this->comps.real);
		comps.push(comparator::_new(cmp, ref));
		return this;
	}
	public: cond* compare(const char* cmp, float ref)
	{
		auto comps = sdd_assume::list(this->comps.real);
		comps.push(comparator::_new(cmp, ref));
		return this;
	}
	public: bool check() const
	{
		auto comps = sdd_assume::list(this->comps.real);
		float val = value();
		bool result = true;

		for (auto i = comps.begin(); result && i != comps.end(); ++i)
		{
			result = result && (**i)(val);
		}

		return result;
	}
	private: float value() const
	{
		ISensor* sensor = this->sensor.real;

		if (sensor != nullptr)
		{
			AnalogSensor* asen = ComponentCaster::analog_sensor(sensor);
			if (asen != nullptr) return asen->read();

			DigitalSensor* dsen = ComponentCaster::digital_sensor(sensor);
			if (dsen != nullptr) return dsen->read();
		}

		return 0;
	}
	
	public: void to_xml(data::OutputStream& stream)
	{
		auto comps = sdd_assume::list(this->comps.real);

		stream.print(F("<condition vid=\""));
		stream.print(sensor.real->ID());
		stream.print(F("\" ctype=\""));
		stream.print(sensor.real->type_str());
		stream.print(F("\">"));

		for (auto i = comps.begin(); i != comps.end(); ++i)
		{
			i->to_xml(stream);
		}

		stream.print(F("</condition>"));
	}
};
