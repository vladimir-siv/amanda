#pragma once

#include <Arduino.h>

#include <dependency.h>
#include <extensions/memory_management/sdd_list.h>

#include "../../common/data/stream.h"

#include "cond.h"

class pack final
{
	private: sdd::type<sdd_list<cond>*> conds;
	private: sdd::type<void*> unused;
	
	public: static pack* _new()
	{
		auto conds = sdd_list<cond>::_new();
		return D::sdds->alloc<pack>(sdd::cast(conds), sdd::cast(nullptr));
	}
	public: pack() :
		conds(sdd_list<cond>::_new()),
		unused(nullptr)
	{ }
	public: ~pack()
	{
		auto conds = sdd_assume::list(this->conds.real);

		for (auto i = conds.begin(); i != conds.end(); ++i)
		{
			i->~cond();
			D::sdds->dealloc(*i);
		}

		conds.clear();
		//D::vlists->dealloc(conds.real);
		this->conds.real = nullptr;
	}
	
	public: void append(cond* cnd)
	{
		auto conds = sdd_assume::list(this->conds.real);
		conds.push(cnd);
	}
	public: bool check() const
	{
		auto conds = sdd_assume::list(this->conds.real);
		bool result = true;

		for (auto i = conds.begin(); result && i != conds.end(); ++i)
		{
			result = result && i->check();
		}

		return result;
	}
	
	public: void to_xml(data::OutputStream& stream)
	{
		auto conds = sdd_assume::list(this->conds.real);

		stream.print(F("<pack>"));

		for (auto i = conds.begin(); i != conds.end(); ++i)
		{
			i->to_xml(stream);
		}

		stream.print(F("</pack>"));
	}
};
