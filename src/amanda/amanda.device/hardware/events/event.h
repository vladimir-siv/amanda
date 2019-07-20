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
	
	public: event() :
		packs(D::vlists->alloc<pack>(D::nodes)),
		act(D::sdds->alloc<activity>(sdd::cast(false), sdd::cast(nullptr)))
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

		D::sdds->dealloc(act.real);
		act.real = nullptr;
	}
	
	public: void append(pack* pck)
	{
		packs.real->push_back(pck);
	}
	public: bool check() const
	{
		bool result = false;

		for (auto i = packs.real->begin(); !result && i != packs.real->end(); ++i)
		{
			result = result || i->check();
		}

		return act.real->setReq(result);
	}
};
