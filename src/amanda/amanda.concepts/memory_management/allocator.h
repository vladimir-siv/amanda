#pragma once

template <typename T>
class Allocator
{
	public: virtual ~Allocator() { };
	public: virtual T* alloc() volatile = 0;
	public: virtual void dealloc(const T*) volatile = 0;
};
