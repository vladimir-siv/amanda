#pragma once

#include <memory_management/specialized/reinterpreting_allocator.h>
#include <structures/specialized/vlist.h>

#include "../../synchronization/mutex.h"
#include "../../synchronization/lock.h"

class VListAllocator
{
	public: virtual ~VListAllocator() { }
	public: virtual unsigned int available() volatile const = 0;
	protected: virtual vlist<void>* _alloc(volatile NodeAllocator* node_allocator) volatile = 0;
	protected: virtual void _dealloc(vlist<void>* object) volatile = 0;
	
	public: template <typename T> vlist<T>* alloc(volatile NodeAllocator* node_allocator) volatile
	{
		return reinterpret_cast<vlist<T>*>(_alloc(node_allocator));
	}
	public: template <typename T> void dealloc(vlist<T>* object) volatile
	{
		_dealloc(reinterpret_cast<vlist<void>*>(object));
	}
};

template <size_t size>
class VListAllocatorT : public VListAllocator
{
	protected: volatile mutex mtx;
	protected: volatile ReinterpretingAllocator<size, vlist<void>, unsigned int, void*, void*, volatile void*> _allocator;
	
	public: virtual unsigned int available() volatile const override { return _allocator.available(); }
	protected: virtual vlist<void>* _alloc(volatile NodeAllocator* node_allocator) volatile override
	{
		lock lck(mtx);
		return _allocator.alloc(0, nullptr, nullptr, node_allocator);
	}
	protected: virtual void _dealloc(vlist<void>* object) volatile override
	{
		lock lck(mtx);
		_allocator.dealloc(object);
	}
};
