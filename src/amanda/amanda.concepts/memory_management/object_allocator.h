#pragma once

#include "../assert/static.h"
#include "../types/def.h"
#include "../structures/bitvector.h"

#include "allocator.h"

template <typename T, size_t size = 32>
class ObjectAllocator : public Allocator<T>
{
	private: struct Chunk
	{
		volatile Chunk* next;
		volatile byte _pad[sizeof(T) - sizeof(next)];
	};
	
	private: static inline void __constraint__() { xassert(sizeof(T) >= sizeof(Chunk*)); xassert(sizeof(T) == sizeof(Chunk)); xassert(size > 0); }
	
	ObjectAllocator(const ObjectAllocator&) = delete;
	ObjectAllocator(ObjectAllocator&&) = delete;
	ObjectAllocator& operator=(const ObjectAllocator&) = delete;
	ObjectAllocator& operator=(ObjectAllocator&&) = delete;

	private: volatile byte _memory[size * sizeof(T)];
	private: volatile Chunk* _first = nullptr;
	private: volatile unsigned int _available = 0;
	
	public: ObjectAllocator() { __constraint__(); format(); }
	
	// Warning: use with caution! Formatting the memory WILL earase & deallocate the entire memory!
	public: void format() volatile
	{
		volatile Chunk* memory = (volatile Chunk*)_memory;

		_first = memory;

		for (volatile unsigned int i = 0; i < size - 1; ++i)
		{
			memory = memory->next = memory + 1;
		}

		memory->next = nullptr;

		_available = size;
	}
	
	public: unsigned int available() volatile const { return _available; }
	
	public: virtual T* alloc() volatile override
	{
		if (_first == nullptr) return nullptr;
		volatile Chunk* chunk = _first;

		_first = _first->next;
		--_available;

		return (T*)chunk;
	}
	public: virtual void dealloc(const T* object) volatile override
	{
		if (object == nullptr) return;

		auto k = object - (T*)_memory;
		
		if (0 <= k && k < size)
		{
			volatile Chunk* chunk = (volatile Chunk*)object;

			chunk->next = _first;
			_first = chunk;
			++_available;
		}
	}
};
