#pragma once

#include "tuple_allocator.h"

#include "../../assert/static.h"

template <typename T, typename... Types>
class ReinterpretingAllocator final
{
	private: void __constraints__() volatile const { xassert(sizeof(T) == sizeof(Tuple<Types...>)); }
	
	private: volatile TupleAllocator<Types...> _allocator;
	
	public: ReinterpretingAllocator() { __constraints__(); }
	
	public: unsigned int available() volatile const { return _allocator.available(); }
	
	public: template <typename... Args> T* alloc(Args&&... arg) volatile
	{
		return reinterpret_cast<T*>(_allocator.alloc(std::forward<Args>(arg)...));
	}
	public: void dealloc(T* object) volatile
	{
		_allocator.dealloc(reinterpret_cast<Tuple<Types...>*>(object));
	}
};
