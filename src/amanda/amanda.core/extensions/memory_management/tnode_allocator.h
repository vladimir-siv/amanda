#pragma once

#include <structures/specialized/vlist.h>

#include "../../synchronization/mutex.h"
#include "../../synchronization/lock.h"

template <size_t size>
class TNodeAllocator : public VNodeAllocator<size>
{
	protected: volatile mutex mtx;
	
	protected: virtual NodeAllocator::Node* _alloc() volatile override
	{
		lock lck(mtx);
		return VNodeAllocator<size>::_alloc();
	}
	protected: virtual void _dealloc(NodeAllocator::Node* object) volatile override
	{
		lock lck(mtx);
		VNodeAllocator<size>::_dealloc(object);
	}
};
