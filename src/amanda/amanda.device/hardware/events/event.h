#pragma once

#include <Arduino.h>

#include <dependency.h>
#include <structures/specialized/vlist.h>

#include "../../common/data/stream.h"

#include "pack.h"
#include "activity.h"

class event final
{
	private: sdd::type<vlist<pack>*> packs;
	private: sdd::type<activity*> acty;
	
	public: static event* _new(long repeat = 0)
	{
		auto packs = D::vlists->alloc<pack>(D::nodes);
		auto act = activity::_new(repeat);
		return D::sdds->alloc<event>(sdd::cast(packs), sdd::cast(act));
	}
	public: event(long repeat = 0) :
		packs(D::vlists->alloc<pack>(D::nodes)),
		acty(activity::_new(repeat))
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

		acty.real->~activity();
		D::sdds->dealloc(acty.real);
		acty.real = nullptr;
	}
	
	public: long getRepeat() const { if (acty.real) return acty.real->getRepeat(); return -1; }
	public: bool setRepeat(long repeat) { if (acty.real) acty.real->setRepeat(repeat); }
	
	public: void append(pack* pck)
	{
		if (packs.real) packs.real->push_back(pck);
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
		if (packs.real == nullptr) return false;

		bool result = false;

		for (auto i = packs.real->begin(); !result && i != packs.real->end(); ++i)
		{
			result = result || i->check();
		}

		return acty.real->setReq(result);
	}
	
	public: void to_xml(data::OutputStream& stream)
	{
		stream.print(F("<event repeat=\"")); stream.print(getRepeat()); stream.print(F("\">"));

		stream.print(F("<requirements>"));
		for (auto i = packs.real->begin(); i != packs.real->end(); ++i)
		{
			i->to_xml(stream);
		}
		stream.print(F("</requirements>"));

		acty.real->to_xml(stream);

		stream.print(F("</event>"));
	}
};
