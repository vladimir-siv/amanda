#pragma once

#include <structures/specialized/vlist.h>

#include "../storage/sdcard.h"

#include "../../hardware/components/abstraction/component.h"
#include "../../hardware/hardware_controller.h"
#include "../../hardware/events.h"
#include "../../common/data/stream.h"
#include "../../lib/string64.h"

#include "event_handle.h"
#include "event_parser.h"

class EventHandler final
{
	public: class StorableInfo
	{
		protected: class Store final
		{
			friend class StorableInfo;

			private: File& file;
			
			private: Store(File& file) : file(file) { }
			
			public: template <typename T1, typename T2> void save(T1&& attr, T2&& value)
			{
				file.print(attr);
				file.print(F(": "));
				file.println(value);
			}
		};
		
		public: virtual ~StorableInfo() { }
		
		public: template <typename T> void save(T&& file)
		{
			if (SD.exists(file)) SD.remove(file);

			File f = SD.open(file, FILE_WRITE);
			f.println(F("[event.store]"));
			f.println();
			Store store(f);
			_save(store);
			f.close();
		}
		public: template <typename T> void load(T&& file)
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
					if (strcmp_P(str.c_str(), PSTR("[event.store]")) != 0) return;
					break;
				}
			}

			unsigned int colon = 0;
			bool colonFound = false;

			while (f.available())
			{
				char c = f.read();
				if (c == '\r') continue;

				if (c != '\n')
				{
					if (c != ':')
					{
						if (!colonFound) ++colon;
					}
					else colonFound = true;

					str += c;
				}
				else
				{
					if (str.empty()) continue;

					if (colon < str.length() && str[colon + 1] == ' ')
					{
						const char* attr = str.c_str();
						((char*)attr)[colon] = 0;

						const char* value = attr + colon + 2;

						_load(attr, value);

						((char*)attr)[colon] = ':';
					}

					str.clear();

					colon = 0;
					colonFound = false;
				}
			}

			f.close();
		}
		
		public: virtual void reset() { }
		protected: virtual void _save(Store& store) = 0;
		protected: virtual void _load(const char* attr, const char* value) = 0;
	};
	
	public: class EventHandlerInfo final : public StorableInfo
	{
		public: unsigned long designator = 0;
		
		public: virtual ~EventHandlerInfo() { }
		
		public: virtual void reset() override
		{
			designator = 0;
		}
		protected: virtual void _save(Store& store) override
		{
			store.save(F("designator"), designator);
		}
		protected: virtual void _load(const char* attr, const char* value) override
		{
			if (strcmp_P(attr, PSTR("")) == 0) ;
			else if (strcmp_P(attr, PSTR("designator")) == 0)
			{
				designator = strtoul(value, nullptr, 0);
			}
		}
	};
	public: class EventDetails final : public StorableInfo
	{
		public: unsigned long id;
		public: const char* name;
		
		public: EventDetails(unsigned long id, const char* name) : id(id), name(name) { }
		public: virtual ~EventDetails() { }
		
		public: virtual void reset() override
		{
			id = 0;
			name = nullptr;
		}
		protected: virtual void _save(Store& store) override
		{
			store.save(F("id"), id);
			if (name == nullptr || strcmp_P(name, PSTR("")) == 0) store.save(F("name"), F("@unnamed"));
			else store.save(F("name"), name);
		}
		protected: virtual void _load(const char* attr, const char* value) override
		{
			if (strcmp_P(attr, PSTR("")) == 0) ;
			else if (strcmp_P(attr, PSTR("id")) == 0)
			{
				id = strtoul(value, nullptr, 0);
			}
			else if (strcmp_P(attr, PSTR("name")) == 0)
			{
				name = value;
			}
		}
	};
	
	public: static EventHandler& instance()
	{
		static EventHandler handler(F("/events"), F("/events/info.sys"));
		return handler;
	}
	
	public: static const __FlashStringHelper* EV_DETAILS_INFO() { return F("/info.xdt"); }
	public: static const __FlashStringHelper* EV_CONTENT() { return F("/event.xml"); }
	
	public: const __FlashStringHelper* const EV_ROOT_DIR;
	public: const __FlashStringHelper* const EV_SYS_INFO;
	
	private: EventHandlerInfo info;
	private: vlist<event_handle> events;
	
	private: EventHandler(const __FlashStringHelper* ev_root_dir, const __FlashStringHelper* ev_sys_info) :
		EV_ROOT_DIR(ev_root_dir), EV_SYS_INFO(ev_sys_info)
	{ }
	
	private: event_handle* create(const event* e, const char* name = nullptr)
	{
		String64 path;
		path += EV_ROOT_DIR;
		path += '/';

		unsigned int len = path.length();
		unsigned long id;

		while (true)
		{
			id = ++info.designator;
			if (id == 0) id = ++info.designator;

			path.append(id, 36);
			if (!SD.exists(path.c_str())) break;
			path.cut(len);
		}

		info.save(EV_SYS_INFO);

		SD.mkdir(path.c_str());

		len = path.length();
		path += EV_DETAILS_INFO();

		EventDetails details(id, name);
		details.save(path.c_str());
		
		path.cut(len);
		path += EV_CONTENT();

		storage::OutputFileStream fs(path.c_str());
		((event*)e)->to_xml(fs);
		
		return event_handle::_new(e, id);
	}
	private: bool destroy(unsigned long id)
	{
		String64 path;
		path += EV_ROOT_DIR;
		path += '/';
		path.append(id, 36);

		if (!SD.exists(path.c_str())) return false;

		unsigned int len = path.length();

		path += EV_DETAILS_INFO();

		if (!SD.remove(path.c_str())) return false;

		path.cut(len);
		path += EV_CONTENT();

		if (!SD.remove(path.c_str())) return false;

		path.cut(len);

		if (!SD.rmdir(path.c_str())) return false;

		return true;
	}
	
	public: void init_storage()
	{
		if (!SD.exists(EV_ROOT_DIR))
		{
			SD.mkdir(EV_ROOT_DIR);
		}
		
		if (!SD.exists(EV_SYS_INFO))
		{
			info.reset();
			info.save(EV_SYS_INFO);
		}
		else info.load(EV_SYS_INFO);
	}
	public: void load_events(HardwareController& controller)
	{
		String64 path;
		path += EV_ROOT_DIR;
		path += '/';
		unsigned int len = path.length();

		File root = SD.open(EV_ROOT_DIR);

		if (root.isDirectory())
		{
			EventParser ev_parser;
			ev_parser.controller = &controller;

			root.rewindDirectory();

			while (true)
			{
				File entry = root.openNextFile();
				if (!entry) break;

				if (entry.isDirectory())
				{
					unsigned long id = String64::parse(entry.name(), 36);

					if (id != 0)
					{
						path += entry.name();
						path += EV_CONTENT(); // maybe optimize this?

						storage::InputFileStream fs(path.c_str());

						if (ev_parser.parse(fs, 0, true))
						{
							event_handle* handle = event_handle::_new(ev_parser.extractEvent(), id);
							events.push_back(handle);
						}

						path.cut(len);
					}
				}

				entry.close();
			}
		}

		root.close();
	}
	public: bool clean_storage()
	{
		bool succ = true;

		succ = succ && SD.remove(EV_SYS_INFO);

		if (SD.exists(EV_ROOT_DIR))
		{
			File root = SD.open(EV_ROOT_DIR);

			bool is_dir = root.isDirectory();
			
			if (is_dir)
			{
				root.rewindDirectory();

				while (true)
				{
					File entry = root.openNextFile();
					if (!entry) break;

					if (entry.isDirectory())
					{
						unsigned long id = String64::parse(entry.name(), 36);
						succ = succ && destroy(id);
					}

					entry.close();
				}
			}

			root.close();

			if (is_dir) succ = succ && SD.rmdir(EV_ROOT_DIR);
			else succ = false;
		}

		info.reset();

		return succ;
	}
	
	public: unsigned long designator() const { unsigned long val = info.designator + 1; if (val == 0) ++val; return val; }
	
	public: void append(const event* e, const char* name = nullptr)
	{
		event_handle* handle = create(e, name);
		events.push_back(handle);
	}
	public: bool remove(unsigned long id)
	{
		if (destroy(id))
		{
			if (events.size() == 0) return false;

			event_handle* handle = nullptr;

			auto prev = events.end();
			for (auto i = events.begin(); i != events.end(); prev = i, ++i)
			{
				if (i->id() == id)
				{
					handle = *i;
					break;
				}
			}

			if (prev != events.end())
			{
				if (handle == nullptr) return false;
				handle->~event_handle();
				D::sdds->dealloc(handle);
				return prev.removeNext(); // should always return true
			}
			else
			{
				handle = events.pop_front();
				handle->~event_handle();
				D::sdds->dealloc(handle);
				return true;
			}
		}

		return false;
	}
	public: void check_events() const
	{
		for (auto i = events.cbegin(); i != events.cend(); ++i)
		{
			i->e()->check();
		}
	}
	
	public: void output_events(data::OutputStream&& stream) const { output_events(stream); }
	public: void output_events(data::OutputStream& stream) const
	{
		for (auto i = events.cbegin(); i != events.cend(); ++i)
		{
			i->e()->to_xml(stream);
		}
	}
};
