#pragma once

#include "reinterpreting_allocator.h"

// [Unused]
template <typename T, typename... Types>
class CastingAllocator final
{
	private: volatile IReinterpretingAllocator<T, Types...>* _allocator;
	
	public: CastingAllocator(volatile IReinterpretingAllocator<T, Types...>* allocator) : _allocator(allocator) { }
	
	public: unsigned int available() volatile const { return _allocator.available(); }
	
	public: template <typename U, typename... Args> U* alloc(Args&&... arg) volatile
	{
		xassert(sizeof(U) == sizeof(T));
		return (U*)_allocator.alloc(std::forward<Args>(arg)...);
	}
	public: template <typename U> void dealloc(U* object) volatile
	{
		xassert(sizeof(U) == sizeof(T));
		_allocator.dealloc((T*)object);
	}
};
