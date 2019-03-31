#pragma once

#include "../types/def.h"
#include "../structures/bitvector.h"

#include "allocator.h"

template <typename T, size_t size = 32>
class ObjectAllocator : public Allocator<T>
{
	ObjectAllocator(const ObjectAllocator&) = delete;
	ObjectAllocator(ObjectAllocator&&) = delete;
	ObjectAllocator& operator=(const ObjectAllocator&) = delete;
	ObjectAllocator& operator=(ObjectAllocator&&) = delete;

	private: byte _memory[size * sizeof(T)];
	private: bitvector<size> _usage;
	private: unsigned int _ptr = size - 1;
	
	public: ObjectAllocator() { }
	
	public: virtual T* alloc() override
	{
		for (unsigned int i = 0; i < size; ++i)
		{
			if (++_ptr == size) _ptr = 0;

			if (!_usage.isset(_ptr))
			{
				_usage.set(_ptr);
				return ((T*)_memory) + _ptr;
			}
		}

		return nullptr;
	}
	public: virtual void dealloc(const T* object)
	{
		if (object == nullptr) return;

		auto k = object - (T*)_memory;
		
		if (0 <= k && k < size)
		{
			_usage.unset(k);
		}
	}
};
