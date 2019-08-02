#pragma once

#include <Arduino.h>

#include <dependency.h>

#include "../../common/data/stream.h"

#include "actions.h"

class record final
{
	private: sdd::type<actions*> acts;
	private: sdd::type<long> rep;
	
	public: static record* _new(long repeat = 0)
	{
		return D::sdds->alloc<record>(sdd::cast(actions::_new()), sdd::cast(repeat));
	}
	public: record(long repeat = 0) :
		acts(actions::_new()),
		rep(repeat)
	{ }
	public: ~record()
	{
		acts.real->~actions();
		D::sdds->dealloc(acts.real);
		acts.real = nullptr;
		rep.real = -1;
	}
	
	public: long getRepeat() const { return rep.real; }
	public: bool setRepeat(long repeat) { rep.real = repeat; }
	
	public: bool occur(bool count)
	{
		if (rep.real < 0) return false;
		if (rep.real == 0) return true;

		if (count)
		{
			--rep.real;
			if (rep.real == 0) rep.real = -1;
		}

		return true;
	}
	
	public: void appendRaise(action* act)
	{
		if (acts.real) acts.real->appendRaise(act);
	}
	public: void appendExpire(action* act)
	{
		if (acts.real) acts.real->appendExpire(act);
	}
	
	public: void raise() const
	{
		if (acts.real) acts.real->raise();
	}
	public: void expire() const
	{
		if (acts.real) acts.real->expire();
	}
	
	public: void to_xml(data::OutputStream& stream)
	{
		acts.real->to_xml(stream);
	}
};
