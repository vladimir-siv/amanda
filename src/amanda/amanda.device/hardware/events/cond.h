#pragma once

#include <Arduino.h>

#include <dependency.h>
#include <structures/specialized/vlist.h>

#include "../components.h"

#include "comparator.h"

class cond final
{
	private: sdd::type<ISensor*> sensor;
	private: sdd::type<vlist<comparator>*> comps;
	
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
	
	public: void append(const char* cmp, float ref)
	{
		comparator* comp = D::sdds->alloc<comparator>(sdd::cast(comparator::resolve(cmp)), sdd::cast(ref));
		comps.real->push_back(comp);
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
};
