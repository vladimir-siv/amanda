#pragma once

#include <Arduino.h>

#include <dependency.h>

class comparator final
{
	using cmp = bool (*)(float val, float ref);
	
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
		char cname[] = "???";
		for (int i = 0; i < sizeof(cname) / sizeof(char) - 1; ++i) cname[i] = name[i];
		if (strcmp(cname, "equ") == 0) return equ();
		if (strcmp(cname, "neq") == 0) return neq();
		if (strcmp(cname, "gtr") == 0) return gtr();
		if (strcmp(cname, "lss") == 0) return lss();
		if (strcmp(cname, "geq") == 0) return geq();
		if (strcmp(cname, "leq") == 0) return leq();
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
	
	public: bool operator()(float val) const { if (_compare.real == nullptr) return false; return _compare.real(val, _ref.real); }
};
