#pragma once

template <typename T>
class enumerator
{
	public: virtual T& operator*() const = 0;
	public: virtual T& operator->() const = 0;
	public: virtual enumerator& operator++() = 0;
	public: virtual enumerator& operator--() { return *this; };
	public: virtual bool operator==(const enumerator& other) const = 0;
	public: virtual bool operator!=(const enumerator& other) const = 0;
	public: virtual ~enumerator() { }
	public: virtual enumerator* clone() const { return nullptr; }
};
