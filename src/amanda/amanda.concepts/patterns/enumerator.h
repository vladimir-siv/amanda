#pragma once

template <typename T>
class Enumerator
{
	public: virtual T& operator*() const = 0;
	public: virtual T& operator->() const = 0;
	public: virtual Enumerator& operator++() = 0;
	public: virtual Enumerator& operator--() { return *this; };
	public: virtual bool operator==(const Enumerator& other) const = 0;
	public: virtual bool operator!=(const Enumerator& other) const = 0;
	public: virtual ~Enumerator() { }
	public: virtual Enumerator* clone() const { return nullptr; }
};
