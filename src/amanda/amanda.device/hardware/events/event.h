#pragma once

#include <Arduino.h>

#include <dependency.h>
#include <structures/specialized/vlist.h>

#include "pack.h"
#include "activity.h"

class event final
{
	private: sdd::type<vlist<pack>*> packs;
	private: sdd::type<activity*> act;
	
	public: static event* _new(int repeat = 0)
	{
		auto packs = D::vlists->alloc<pack>(D::nodes);
		auto act = activity::_new(repeat);
		return D::sdds->alloc<event>(sdd::cast(packs), sdd::cast(act));
	}
	public: event(int repeat = 0) :
		packs(D::vlists->alloc<pack>(D::nodes)),
		act(activity::_new(repeat))
	{ }
	public: ~event()
	{
		for (auto i = packs.real->begin(); i != packs.real->end(); ++i)
		{
			i->~pack();
			D::sdds->dealloc(*i);
		}

		packs.real->clear();
		D::vlists->dealloc(packs.real);
		packs.real = nullptr;

		act.real->~activity();
		D::sdds->dealloc(act.real);
		act.real = nullptr;
	}
	
	public: int getRepeat() const { if (act.real) return act.real->getRepeat(); return -1; }
	public: bool setRepeat(int repeat) { if (act.real) act.real->setRepeat(repeat); }
	
	public: void append(pack* pck)
	{
		if (packs.real) packs.real->push_back(pck);
	}
	
	public: void appendRaise(action* actn)
	{
		if (act.real) act.real->appendRaise(actn);
	}
	public: void appendExpire(action* actn)
	{
		if (act.real) act.real->appendExpire(actn);
	}
	
	public: bool check() const
	{
		if (packs.real == nullptr) return false;

		bool result = false;

		for (auto i = packs.real->begin(); !result && i != packs.real->end(); ++i)
		{
			result = result || i->check();
		}

		return act.real->setReq(result);
	}
};
