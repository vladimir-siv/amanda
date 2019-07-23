#pragma once

#include <Arduino.h>

#include <dependency.h>

#include "record.h"

class activity final
{
	private: sdd::type<bool> req;
	private: sdd::type<record*> rec;
	
	public: static activity* _new(int repeat = 0)
	{
		return D::sdds->alloc<activity>(sdd::cast(false), sdd::cast(record::_new(repeat)));
	}
	public: activity(int repeat = 0) :
		req(false),
		rec(record::_new(repeat))
	{ }
	public: ~activity()
	{
		req.real = false;
		rec.real->~record();
		D::sdds->dealloc(rec.real);
		rec.real = nullptr;
	}
	
	public: int getRepeat() const { if (rec.real) return rec.real->getRepeat(); return -1; }
	public: bool setRepeat(int repeat) { if (rec.real) rec.real->setRepeat(repeat); }
	
	public: bool setReq(bool value)
	{
		if (req.real != value)
		{
			req.real = value;
			if (value) raise();
			else expire();
		}

		return value;
	}
	
	public: void appendRaise(action* act)
	{
		if (rec.real) rec.real->appendRaise(act);
	}
	public: void appendExpire(action* act)
	{
		if (rec.real) rec.real->appendExpire(act);
	}
	
	private: void raise()
	{
		if (rec.real)
		{
			if (!rec.real->occur(false)) return;
			rec.real->raise();
		}
	}
	private: void expire()
	{
		if (rec.real)
		{
			if (!rec.real->occur(true)) return;
			rec.real->expire();
		}
	}
};
