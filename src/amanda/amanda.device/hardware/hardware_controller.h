#pragma once

#include "components/abstraction/component.h"
#include "components/abstraction/sensor.h"
#include "components/abstraction/element.h"
#include "../xml/api.h"
#include "../lib/string_builder.h"

#include <structures/vector.h>

class HardwareController
{
	protected: vector<IComponent*> _components;
	protected: StringBuilder<144> _scan_builder;
	
	public: HardwareController(unsigned int capacity) : _components(capacity) { }
	public: virtual ~HardwareController() { }
	
	public: unsigned int capacity() const { return _components.capacity(); }
	public: unsigned int size() const { return _components.size(); }
	public: IComponent* operator[](int i) const
	{
		if (i < 0 || size() <= i) return nullptr;
		return const_cast<IComponent*>(_components[i]);
	}
	
	public: HardwareController& operator+=(IComponent* component)
	{
		_scan_builder.append("<component vid=\"");
		_scan_builder.append(component->ID());
		_scan_builder.append("\" ctype=\"");
		_scan_builder.append(component->type_str());
		_scan_builder.append("\" description=\"");
		_scan_builder.append(component->description());
		_scan_builder.append("\" commands=\"");
		_scan_builder.append(component->commands());
		_scan_builder.append("\">");
		
		if (component->is_any(IComponent::ELEMENT))
		{
			_scan_builder.map(_components.size(), _scan_builder.capacity());
		}
		else if (component->is(IComponent::DigitalSensor))
		{
			_scan_builder.append("<state>");
			_scan_builder.map(_components.size(), _scan_builder.size());
			_scan_builder.append((unsigned long)0);
			_scan_builder.append("</state>");
		}
		else if (component->is(IComponent::AnalogSensor))
		{
			AnalogSensor* asen = (AnalogSensor*)component;

			_scan_builder.append("<value unit=\"");
			_scan_builder.append(asen->unit());
			_scan_builder.append("\">");
			_scan_builder.map(_components.size(), _scan_builder.size());
			_scan_builder.append(0.0f);
			_scan_builder.append("</value>");
		}

		_scan_builder.append("</component>");

		_components.push(component);
		return *this;
	}
	
	public: template <typename T> T fetch(int i) const { return (T)(*this)[i]; }
	
	public: ISensor* sensor(int i) const
	{
		IComponent* comp = (*this)[i];
		
		if (comp != nullptr && comp->is_any(IComponent::Type::SENSOR))
		{
			return (ISensor*)comp;
		}

		return nullptr;
	}
	public: IElement* element(int i) const
	{
		IComponent* comp = (*this)[i];

		if (comp != nullptr && comp->is_any(IComponent::Type::ELEMENT))
		{
			return (IElement*)comp;
		}

		return nullptr;
	}
	
	public: DigitalSensor* digital_sensor(int i) const
	{
		IComponent* comp = (*this)[i];

		if (comp != nullptr && comp->is((IComponent::Type)(IComponent::DIGITAL | IComponent::SENSOR)))
		{
			return (DigitalSensor*)comp;
		}

		return nullptr;
	}
	public: AnalogSensor* analog_sensor(int i) const
	{
		IComponent* comp = (*this)[i];

		if (comp != nullptr && comp->is((IComponent::Type)(IComponent::ANALOG | IComponent::SENSOR)))
		{
			return (AnalogSensor*)comp;
		}

		return nullptr;
	}
	public: DigitalElement* digital_element(int i) const
	{
		IComponent* comp = (*this)[i];

		if (comp != nullptr && comp->is((IComponent::Type)(IComponent::DIGITAL | IComponent::ELEMENT)))
		{
			return (DigitalElement*)comp;
		}

		return nullptr;
	}
	public: AnalogElement* analog_element(int i) const
	{
		IComponent* comp = (*this)[i];

		if (comp != nullptr && comp->is((IComponent::Type)(IComponent::ANALOG | IComponent::ELEMENT)))
		{
			return (AnalogElement*)comp;
		}

		return nullptr;
	}
	
	public: virtual const StringBuilder<144>& scan()
	{
		for (int i = 0; i < size(); ++i)
		{
			DigitalSensor* dsen = digital_sensor(i);
			if (dsen != nullptr)
			{
				_scan_builder.set(_scan_builder.map(i), (unsigned long)dsen->read());
				continue;
			}

			AnalogSensor* asen = analog_sensor(i);
			if (asen != nullptr)
			{
				_scan_builder.set(_scan_builder.map(i), (float)asen->read());
				continue;
			}
		}

		return _scan_builder;
	}
};
