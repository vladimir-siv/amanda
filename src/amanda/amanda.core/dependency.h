#pragma once

#include "extensions/memory_management/tnode_allocator.h"
#include "extensions/memory_management/vlist_allocator.h"
#include "extensions/memory_management/sdd_allocator.h"

namespace dependency
{
	volatile NodeAllocator* node_allocator();
	volatile VListAllocator* vlist_allocator();
	volatile SDDAllocator* sdd_allocator();
}

namespace D
{
	extern volatile NodeAllocator* nodes;
	extern volatile VListAllocator* vlists;
	extern volatile SDDAllocator* sdds;
};
