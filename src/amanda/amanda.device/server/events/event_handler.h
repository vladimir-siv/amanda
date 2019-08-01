#pragma once

#include <structures/specialized/vlist.h>

#include "../storage/sdcard.h"

#include "../../hardware/components/abstraction/component.h"
#include "../../hardware/hardware_controller.h"
#include "../../hardware/events.h"
#include "../../lib/string64.h"

class EventHandler final
{
	public: struct EventHandlerInfo final
	{
		friend class EventHandler;

		unsigned long designator = 0;
		
		private: void save(const __FlashStringHelper* file)
		{
			if (SD.exists(file)) SD.remove(file);

			File f = SD.open(file, FILE_WRITE);
			f.println(F("[EVENTS.INFO]"));
			f.println();
			f.print(F("designator: ")); f.println(designator);
			f.close();
		}
		private: void load(const __FlashStringHelper* file)
		{
			if (!SD.exists(file)) return;

			File f = SD.open(file, FILE_READ);

			String64 str;

			while (f.available())
			{
				char c = f.read();
				if (c == '\r') continue;
				if (c != '\n') str += c;
				else
				{
					if (strcmp_P(str.c_str(), PSTR("[EVENTS.INFO]")) != 0) return;
					break;
				}
			}

			while (f.available())
			{
				char c = f.read();
				if (c == '\r') continue;
				if (c != '\n') str += c;
				else
				{
					if (str.empty()) continue;

					unsigned int colon = str.index_of(':');

					if (colon < str.length() && str[colon + 1] == ' ')
					{
						const char* attr = str.c_str();
						((char*)attr)[colon] = 0;
						
						const char* value = attr + colon + 2;

						if (strcmp_P(attr, PSTR("")) == 0) ;
						else if (strcmp_P(attr, PSTR("designator")) == 0)
						{
							designator = strtoul(value, nullptr, 0);
						}

						((char*)attr)[colon] = ':';
					}

					str.clear();
				}
			}

			f.close();
		}
	};
	
	public: static EventHandler& instance()
	{
		static EventHandler handler(F("/events"), F("/events/info.sys"));
		return handler;
	}
	
	public: const __FlashStringHelper* const EV_ROOT_DIR;
	public: const __FlashStringHelper* const EV_SYS_INFO;
	
	private: vlist<event> events;
	
	private: EventHandler(const __FlashStringHelper* ev_root_dir, const __FlashStringHelper* ev_sys_info) :
		EV_ROOT_DIR(ev_root_dir), EV_SYS_INFO(ev_sys_info)
	{ }
	
	public: void init_storage() const
	{
		if (!SD.exists(EV_ROOT_DIR))
		{
			SD.mkdir(EV_ROOT_DIR);
		}
		
		if (!SD.exists(EV_SYS_INFO))
		{
			EventHandlerInfo info;
			info.save(EV_SYS_INFO);
		}
	}
	public: bool clean_storage() const
	{
		bool succ = true;

		if (SD.exists(EV_ROOT_DIR))
		{
			succ = storage::SDCard::rmdir_f(EV_ROOT_DIR);
		}

		if (SD.exists(EV_SYS_INFO))
		{
			SD.remove(EV_SYS_INFO);
		}

		return succ;
	}
	
	public: void append(const event* e)
	{
		events.push_back(e);
	}
	public: void check_events() const
	{
		for (auto i = events.cbegin(); i != events.cend(); ++i)
		{
			i->check();
		}
	}
};
