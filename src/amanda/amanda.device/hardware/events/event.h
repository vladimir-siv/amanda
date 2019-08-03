#pragma once

#include <Arduino.h>

#include <dependency.h>
#include <extensions/memory_management/sdd_list.h>

#include "../../common/data/stream.h"

#include "pack.h"
#include "activity.h"

class event final
{
	private: sdd::type<sdd_list<pack>*> packs;
	private: sdd::type<activity*> acty;
	
	public: static event* _new(long repeat = 0)
	{
		auto packs = sdd_list<pack>::_new();
		auto act = activity::_new(repeat);
		return D::sdds->alloc<event>(sdd::cast(packs), sdd::cast(act));
	}
	public: event(long repeat = 0) :
		packs(sdd_list<pack>::_new()),
		acty(activity::_new(repeat))
	{ }
	public: ~event()
	{
		auto packs = sdd_assume::list(this->packs.real);
		
		for (auto i = packs.begin(); i != packs.end(); ++i)
		{
			i->~pack();
			D::sdds->dealloc(*i);
		}

		packs.clear();
		//D::vlists->dealloc(packs.real);
		this->packs.real = nullptr; // clear() should also do this

		acty.real->~activity();
		D::sdds->dealloc(acty.real);
		acty.real = nullptr;
	}
	
	public: long getRepeat() const { if (acty.real) return acty.real->getRepeat(); return -1; }
	public: bool setRepeat(long repeat) { if (acty.real) acty.real->setRepeat(repeat); }
	
	public: void append(pack* pck)
	{
		auto packs = sdd_assume::list(this->packs.real);
		packs.push(pck);
	}
	
	public: void appendRaise(action* act)
	{
		if (acty.real) acty.real->appendRaise(act);
	}
	public: void appendExpire(action* act)
	{
		if (acty.real) acty.real->appendExpire(act);
	}
	
	public: bool check() const
	{
		auto packs = sdd_assume::list(this->packs.real);
		bool result = false;

		for (auto i = packs.begin(); !result && i != packs.end(); ++i)
		{
			result = result || i->check();
		}

		return acty.real->setReq(result);
	}
	
	public: void to_xml(data::OutputStream& stream)
	{
		auto packs = sdd_assume::list(this->packs.real);

		stream.print(F("<event repeat=\"")); stream.print(getRepeat()); stream.print(F("\">"));

		stream.print(F("<requirements>"));
		for (auto i = packs.begin(); i != packs.end(); ++i)
		{
			i->to_xml(stream);
		}
		stream.print(F("</requirements>"));

		acty.real->to_xml(stream);

		stream.print(F("</event>"));
	}
};
