#pragma once

#include "components.h"
#include "../xml/api.h"
#include "../lib/string_builder.h"

#include <structures/specialized/vlist.h>

class HardwareController
{
	protected: vlist<IComponent> _components;
	protected: StringBuilder<144> _scan_builder;
	
	public: virtual ~HardwareController() { }
	
	public: unsigned int size() const { return _components.size(); }
	
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

		_components.push_back(component);
		return *this;
	}
	
	public: virtual const StringBuilder<144>& scan()
	{
		auto it = _components.begin();
		auto end = _components.end();
		
		for (unsigned int i = 0; it != end; ++it, ++i)
		{
			DigitalSensor* dsen = ComponentCaster::digital_sensor(*it);
			if (dsen != nullptr)
			{
				_scan_builder.set(_scan_builder.map(i), (unsigned long)dsen->read());
				continue;
			}

			AnalogSensor* asen = ComponentCaster::analog_sensor(*it);
			if (asen != nullptr)
			{
				_scan_builder.set(_scan_builder.map(i), (float)asen->read());
				continue;
			}
		}

		return _scan_builder;
	}
};
