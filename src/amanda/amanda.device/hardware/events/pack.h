#pragma once

#include <Arduino.h>

#include <dependency.h>
#include <structures/specialized/vlist.h>

#include "cond.h"

class pack final
{
	private: sdd::type<vlist<cond>*> conds;
	private: sdd::type<void*> unused;
	
	public: static pack* _new()
	{
		auto conds = D::vlists->alloc<cond>(D::nodes);
		return D::sdds->alloc<pack>(sdd::cast(conds), sdd::cast(nullptr));
	}
	public: pack() :
		conds(D::vlists->alloc<cond>(D::nodes)),
		unused(nullptr)
	{ }
	public: ~pack()
	{
		for (auto i = conds.real->begin(); i != conds.real->end(); ++i)
		{
			i->~cond();
			D::sdds->dealloc(*i);
		}

		conds.real->clear();
		D::vlists->dealloc(conds.real);
		conds.real = nullptr;
	}
	
	public: void append(cond* cnd)
	{
		conds.real->push_back(cnd);
	}
	public: bool check() const
	{
		bool result = true;

		for (auto i = conds.real->begin(); result && i != conds.real->end(); ++i)
		{
			result = result && i->check();
		}

		return result;
	}
};
