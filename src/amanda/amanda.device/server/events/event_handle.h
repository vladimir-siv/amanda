#pragma once

#include <Arduino.h>

#include <dependency.h>

#include "../../hardware/events.h"

class event_handle final
{
	private: sdd::type<event*> _e;
	private: unsigned long _id;
	
	public: static event_handle* _new(const event* e, unsigned long id)
	{
		return D::sdds->alloc<event_handle>(sdd::cast((event*)e), id);
	}
	public: event_handle(event* e, unsigned long id) :
		_e(e),
		_id(id)
	{ }
	public: ~event_handle()
	{
		_e.real->~event();
		D::sdds->dealloc(_e.real);
		_e.real = nullptr;

		_id = 0;
	}
	
	public: event* e() const { return _e.real; }
	public: unsigned long id() const { return _id; }
};
