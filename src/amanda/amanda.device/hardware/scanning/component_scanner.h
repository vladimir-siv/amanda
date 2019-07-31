#pragma once

#include "scanner.h"
#include "../components.h"

class ComponentScanner
{
	protected: Scanner* scanner;
	
	public: ComponentScanner(Scanner* scanner) : scanner(scanner) { }
	public: virtual ~ComponentScanner() { }
	
	public: virtual void scan(IComponent* component)
	{
		if (scanner == nullptr || component == nullptr) return;

		scanner->scan(F("<component vid=\""));
		scanner->scan(component->ID());
		scanner->scan(F("\" ctype=\""));
		scanner->scan(component->type_str());
		scanner->scan(F("\" description=\""));
		scanner->scan(component->description());
		scanner->scan(F("\" commands=\""));
		scanner->scan(component->commands());
		scanner->scan(F("\">"));

		DigitalSensor* dsen = ComponentCaster::digital_sensor(component);
		if (dsen != nullptr)
		{
			scanner->scan(F("<state>"));
			scanner->scan(dsen->read());
			scanner->scan(F("</state>"));
		}

		AnalogSensor* asen = ComponentCaster::analog_sensor(component);
		if (asen != nullptr)
		{
			scanner->scan(F("<value unit=\""));
			unit u = asen->s_unit();
			if (u.scale() != 0) scanner->scan(u.prefix());
			scanner->scan(u.measure()->label);
			scanner->scan(F("\">"));
			scanner->scan(asen->read());
			scanner->scan(F("</value>"));
		}

		scanner->scan(F("</component>"));
	}
};
