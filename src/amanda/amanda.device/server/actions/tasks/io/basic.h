#pragma once

#include <Arduino.h>

#include "../../task.h"

#include "../../../../common/units.h"
#include "../../../../common/data/flash_stream.h"

// template: "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><action task=\"IO/basic\"><arg>{vid}:{ctype}</arg><arg>{operation}</arg><arg>{value unit}</arg></action>"

class BasicIOTask : public Task
{
	public: static BasicIOTask* instance() { static BasicIOTask task; return &task; }
	
	protected: enum Operation { NONE = 0, READ = 1, WRITE = 2 };
	
	protected: unsigned long arg_cnt;
	protected: IComponent* component;
	protected: Operation op;
	protected: double val;
	
	public: virtual ~BasicIOTask() { }
	
	public: virtual void configure(const char* name = nullptr) override
	{
		arg_cnt = 0;
		component = nullptr;
		op = NONE;
		val = 0;
	}
	public: virtual void accept(const char* arg) override
	{
		switch (arg_cnt)
		{
			case 0:
			{
				char* cid = (char*)arg;

				unsigned int colon = 0;
				for (char* i = cid; *i != 0 && *i != ':'; ++i) ++colon;
				
				if (cid[colon] == ':')
				{
					HardwareController* controller = client->get_handling_server()->get_bound_controller();

					if (controller != nullptr)
					{
						cid[colon] = 0;

						component = controller->find(strtoul(cid, nullptr, 0), cid + colon + 1);
						
						cid[colon] = ':';
					}
				}
			} break;
			case 1:
			{
				if (component != nullptr)
				{
					if (strcmp_P(arg, PSTR("")) == 0) op = NONE;
					else if (strcmp_P(arg, PSTR("read")) == 0) op = READ;
					else if (strcmp_P(arg, PSTR("write")) == 0) op = WRITE;
					else op = NONE;
				}
			} break;
			case 2:
			{
				if (component != nullptr && op == WRITE)
				{
					char* value = (char*)arg;

					unsigned int space = 0;
					for (char* i = value; *i != 0 && *i != ' '; ++i) ++space;

					if (value[space] == ' ')
					{
						value[space] = 0;

						val = strtod(value, nullptr);
						unit u(value + space + 1);
						val = u.convert(val);

						value[space] = ' ';
					}
					else val = strtod(value, nullptr);
				}
			} break;
		}

		++arg_cnt;
	}
	public: virtual void invoke() override
	{
		if (component != nullptr && op != NONE)
		{
			switch (op)
			{
				case READ:
				{
					AnalogSensor* asen = ComponentCaster::analog_sensor(component);

					if (asen != nullptr)
					{
						AnalogValue v = asen->read();
						unit u = asen->s_unit();

						client->respond(data::FlashStream(F("<value unit=\"")));
						if (u.scale() != 0) client->respond(u.prefix());
						client->respond(u.measure()->label);
						client->respond(data::FlashStream(F("\">")));
						client->respond(v);
						client->respond(data::FlashStream(F("</value>")));

						return;
					}

					DigitalSensor* dsen = ComponentCaster::digital_sensor(component);

					if (dsen != nullptr)
					{
						DigitalState v = dsen->read();

						client->respond(data::FlashStream(F("<state>")));
						client->respond((unsigned long)v);
						client->respond(data::FlashStream(F("</state>")));
						
						return;
					}

					AnalogElement* ael = ComponentCaster::analog_element(component);

					if (ael != nullptr)
					{
						AnalogValue v = ael->current_value();
						unit u = ael->e_unit();

						client->respond(data::FlashStream(F("<value unit=\"")));
						if (u.scale() != 0) client->respond(u.prefix());
						client->respond(u.measure()->label);
						client->respond(data::FlashStream(F("\">")));
						client->respond(v);
						client->respond(data::FlashStream(F("</value>")));

						return;
					}

					DigitalElement* del = ComponentCaster::digital_element(component);

					if (del != nullptr)
					{
						DigitalState v = del->current_state();

						client->respond(data::FlashStream(F("<state>")));
						client->respond((unsigned long)v);
						client->respond(data::FlashStream(F("</state>")));

						return;
					}
				} break;
				case WRITE:
				{
					AnalogElement* ael = ComponentCaster::analog_element(component);

					if (ael != nullptr)
					{
						ael->write((AnalogValue)val);

						client->respond(data::FlashStream(F("<action>success</action>")));
						return;
					}

					DigitalElement* del = ComponentCaster::digital_element(component);

					if (del != nullptr)
					{
						del->write((DigitalState)val);

						client->respond(data::FlashStream(F("<action>success</action>")));
						return;
					}
				} break;
			}
		}

		client->respond(data::FlashStream(F("<action>failure</action>")));
	}
};
