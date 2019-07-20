#pragma once

#include <assert/static.h>
#include <memory_management/specialized/reinterpreting_allocator.h>

#include "../../synchronization/mutex.h"
#include "../../synchronization/lock.h"

struct spc_dbl_dword
{
	unsigned long dw1, dw2;

	template <typename T> union type
	{
		void __constraint__() { xassert(sizeof(T) <= sizeof(unsigned long)); }

		T real;
		unsigned long pseudo;

		type(T value) : real(value) { __constraint__(); }
		type(unsigned long value) : pseudo(value) { __constraint__(); }
	};

	template <typename T> static unsigned long cast(T val)
	{
		type<T> value(val);
		return value.pseudo;
	}
};

using sdd = spc_dbl_dword;

class SDDAllocator
{
	public: virtual ~SDDAllocator() { }
	public: virtual unsigned int available() volatile const = 0;
	protected: virtual spc_dbl_dword* _alloc(unsigned long dw1 = 0, unsigned long dw2 = 0) volatile = 0;
	protected: virtual void _dealloc(spc_dbl_dword* object) volatile = 0;
	
	public: template <typename T> T* alloc(unsigned long dw1 = 0, unsigned long dw2 = 0) volatile
	{
		xassert(sizeof(T) == sizeof(spc_dbl_dword));
		return (T*)_alloc(dw1, dw2);
	}
	public: template <typename T> void dealloc(T* object) volatile
	{
		xassert(sizeof(T) == sizeof(spc_dbl_dword));
		_dealloc((spc_dbl_dword*)object);
	}
};

template <size_t size>
class SDDAllocatorT : public SDDAllocator
{
	protected: volatile mutex mtx;
	protected: volatile ReinterpretingAllocator<size, spc_dbl_dword, unsigned long, unsigned long> _allocator;
	
	public: virtual unsigned int available() volatile const override { return _allocator.available(); }
	protected: virtual spc_dbl_dword* _alloc(unsigned long dw1 = 0, unsigned long dw2 = 0) volatile override
	{
		lock lck(mtx);
		return _allocator.alloc(dw1, dw2);
	}
	protected: virtual void _dealloc(spc_dbl_dword* object) volatile override
	{
		lock lck(mtx);
		_allocator.dealloc(object);
	}
};
