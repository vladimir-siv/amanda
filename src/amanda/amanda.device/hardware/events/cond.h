#pragma once

#include <Arduino.h>

#include <dependency.h>
#include <structures/specialized/vlist.h>

#include "../../common/data/stream.h"

#include "../components.h"

#include "comparator.h"

class cond final
{
	private: sdd::type<ISensor*> sensor;
	private: sdd::type<vlist<comparator>*> comps;
	
	public: static cond* _new(ISensor* sensor)
	{
		auto comps = D::vlists->alloc<comparator>(D::nodes);
		return D::sdds->alloc<cond>(sdd::cast(sensor), sdd::cast(comps));
	}
	public: cond(ISensor* sensor) :
		sensor(sensor),
		comps(D::vlists->alloc<comparator>(D::nodes))
	{ }
	public: ~cond()
	{
		for (auto i = comps.real->begin(); i != comps.real->end(); ++i) D::sdds->dealloc(*i);
		comps.real->clear();
		D::vlists->dealloc(comps.real);

		sensor.real = nullptr;
		comps.real = nullptr;
	}
	
	public: cond* compare(comparator::cmp cmp, float ref)
	{
		comps.real->push_back(comparator::_new(cmp, ref));
		return this;
	}
	public: cond* compare(const char* cmp, float ref)
	{
		comps.real->push_back(comparator::_new(cmp, ref));
		return this;
	}
	public: bool check() const
	{
		float val = value();
		bool result = true;

		for (auto i = comps.real->begin(); result && i != comps.real->end(); ++i)
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
		stream.print(F("<condition vid=\""));
		stream.print(sensor.real->ID());
		stream.print(F("\" ctype=\""));
		stream.print(sensor.real->type_str());
		stream.print(F("\">"));

		for (auto i = comps.real->begin(); i != comps.real->end(); ++i)
		{
			i->to_xml(stream);
		}

		stream.print(F("</condition>"));
	}
};
