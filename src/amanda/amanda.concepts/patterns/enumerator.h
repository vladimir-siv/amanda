#pragma once

#include "../rtti/type.h"

template <typename T>
class enumerator
{
	__RTTI__
	public: virtual ~enumerator() { }
	public: virtual enumerator* clone() const { return nullptr; }
	public: virtual T& operator*() const = 0;
	public: virtual T& operator->() const = 0;
	public: virtual enumerator& operator++() = 0;
	public: virtual enumerator& operator--() { return *this; };
	public: virtual bool equals(const enumerator& other) const = 0;
	public: friend bool operator==(const enumerator& e1, const enumerator& e2) { return e1.equals(e2); }
	public: friend bool operator!=(const enumerator& e1, const enumerator& e2) { return !(e1 == e2); }
};
