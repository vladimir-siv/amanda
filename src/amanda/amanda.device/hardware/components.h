#pragma once

#include "components/abstraction/component.h"
#include "components/abstraction/sensor.h"
#include "components/abstraction/element.h"

#include "components/tmr.h"
#include "components/btn.h"
#include "components/led.h"
#include "components/lmp.h"
#include "components/buzz.h"
#include "components/pot.h"
#include "components/lm35.h"
#include "components/ldr.h"
#include "components/pir.h"

class ComponentCaster final
{
	private: virtual ~ComponentCaster() = 0;
	
	public: static ISensor* sensor(const IComponent* comp)
	{
		if (comp != nullptr && comp->is_any(IComponent::Type::SENSOR))
		{
			return (ISensor*)comp;
		}

		return nullptr;
	}
	public: static IElement* element(const IComponent* comp)
	{
		if (comp != nullptr && comp->is_any(IComponent::Type::ELEMENT))
		{
			return (IElement*)comp;
		}

		return nullptr;
	}
	
	public: static DigitalSensor* digital_sensor(const IComponent* comp)
	{
		if (comp != nullptr && comp->is((IComponent::Type)(IComponent::DIGITAL | IComponent::SENSOR)))
		{
			return (DigitalSensor*)comp;
		}

		return nullptr;
	}
	public: static AnalogSensor* analog_sensor(const IComponent* comp)
	{
		if (comp != nullptr && comp->is((IComponent::Type)(IComponent::ANALOG | IComponent::SENSOR)))
		{
			return (AnalogSensor*)comp;
		}

		return nullptr;
	}
	public: static DigitalElement* digital_element(const IComponent* comp)
	{
		if (comp != nullptr && comp->is((IComponent::Type)(IComponent::DIGITAL | IComponent::ELEMENT)))
		{
			return (DigitalElement*)comp;
		}

		return nullptr;
	}
	public: static AnalogElement* analog_element(const IComponent* comp)
	{
		if (comp != nullptr && comp->is((IComponent::Type)(IComponent::ANALOG | IComponent::ELEMENT)))
		{
			return (AnalogElement*)comp;
		}

		return nullptr;
	}
};
