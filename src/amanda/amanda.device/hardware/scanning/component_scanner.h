#pragma once

#include "../components.h"

#include "../../common/data/stream.h"

class ComponentScanner
{
	protected: data::OutputStream& output;
	
	public: ComponentScanner(data::OutputStream& output) : output(output) { }
	public: virtual ~ComponentScanner() { }
	
	public: virtual void scan(IComponent* component)
	{
		if (component == nullptr) return;

		output.print(F("<component vid=\""));
		output.print(component->ID());
		output.print(F("\" ctype=\""));
		output.print(component->type_str());
		output.print(F("\" description=\""));
		output.print(component->description());
		output.print(F("\" commands=\""));
		output.print(component->commands());
		output.print(F("\">"));

		DigitalSensor* dsen = ComponentCaster::digital_sensor(component);
		if (dsen != nullptr)
		{
			output.print(F("<state>"));
			output.print(dsen->read());
			output.print(F("</state>"));
		}

		AnalogSensor* asen = ComponentCaster::analog_sensor(component);
		if (asen != nullptr)
		{
			output.print(F("<value unit=\""));
			unit u = asen->s_unit();
			if (u.scale() != 0) output.print(u.prefix());
			output.print(u.measure()->label);
			output.print(F("\">"));
			output.print(asen->read());
			output.print(F("</value>"));
		}

		DigitalElement* del = ComponentCaster::digital_element(component);
		if (dsen != nullptr)
		{
			output.print(F("<state>"));
			output.print(del->current_state());
			output.print(F("</state>"));
		}

		AnalogElement* ael = ComponentCaster::analog_element(component);
		if (ael != nullptr)
		{
			output.print(F("<value unit=\""));
			unit u = ael->e_unit();
			if (u.scale() != 0) output.print(u.prefix());
			output.print(u.measure()->label);
			output.print(F("\">"));
			output.print(ael->current_value());
			output.print(F("</value>"));
		}

		output.print(F("</component>"));
	}
};
