#pragma once

#include <Arduino.h>

#include <dependency.h>

#include "../../common/data/stream.h"

class comparator final
{
	public: using cmp = bool (*)(float val, float ref);
	
	public: static cmp equ()
	{
		static cmp comp = [](float val, float ref) -> bool { return val == ref; };
		return comp;
	}
	public: static cmp neq()
	{
		static cmp comp = [](float val, float ref) -> bool { return val != ref; };
		return comp;
	}
	public: static cmp gtr()
	{
		static cmp comp = [](float val, float ref) -> bool { return val > ref; };
		return comp;
	}
	public: static cmp lss()
	{
		static cmp comp = [](float val, float ref) -> bool { return val < ref; };
		return comp;
	}
	public: static cmp geq()
	{
		static cmp comp = [](float val, float ref) -> bool { return val >= ref; };
		return comp;
	}
	public: static cmp leq()
	{
		static cmp comp = [](float val, float ref) -> bool { return val <= ref; };
		return comp;
	}
	public: static cmp resolve(const char* name)
	{
		char cname[4] = { '?', '?', '?', 0 };
		for (int i = 0; i < sizeof(cname) / sizeof(char) - 1; ++i) cname[i] = name[i];
		if (strcmp_P(cname, PSTR("equ")) == 0) return equ();
		if (strcmp_P(cname, PSTR("neq")) == 0) return neq();
		if (strcmp_P(cname, PSTR("gtr")) == 0) return gtr();
		if (strcmp_P(cname, PSTR("lss")) == 0) return lss();
		if (strcmp_P(cname, PSTR("geq")) == 0) return geq();
		if (strcmp_P(cname, PSTR("leq")) == 0) return leq();
		return nullptr;
	}
	
	private: sdd::type<cmp> _compare;
	private: sdd::type<float> _ref;
	
	public: static comparator* _new(cmp compare, float ref)
	{
		return D::sdds->alloc<comparator>(sdd::cast(compare), sdd::cast(ref));
	}
	public: static comparator* _new(const char* name, float ref)
	{
		return D::sdds->alloc<comparator>(sdd::cast(comparator::resolve(name)), sdd::cast(ref));
	}
	public: comparator(cmp compare, float ref) : _compare(compare), _ref(ref) { }
	public: comparator(const char* name, float ref) : _compare(resolve(name)), _ref(ref) { }
	
	public: bool operator()(float val) const { if (_compare.real == nullptr) return false; return (*_compare.real)(val, _ref.real); }
	
	public: void to_xml(data::OutputStream& stream)
	{
		const __FlashStringHelper* name = nullptr;

		if (_compare.real == nullptr) return;
		else if (_compare.real == equ()) name = F("equ");
		else if (_compare.real == neq()) name = F("neq");
		else if (_compare.real == gtr()) name = F("gtr");
		else if (_compare.real == lss()) name = F("lss");
		else if (_compare.real == geq()) name = F("geq");
		else if (_compare.real == leq()) name = F("leq");
		else return;

		stream.print('<');
		stream.print(name);
		stream.print('>');
		stream.print(_ref.real);
		stream.print(F("</"));
		stream.print(name);
		stream.print('>');
	}
};
