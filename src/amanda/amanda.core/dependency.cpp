#include <Arduino.h>

#include "dependency.h"

namespace dependency
{
	volatile NodeAllocator* node_allocator()
	{
		// number of nodes available in user sketch
		static volatile TNodeAllocator<64> _nodes;
		return &_nodes;
	}

	volatile VListAllocator* vlist_allocator()
	{
		// number of vlists available for vsublists
		static volatile VListAllocatorT<32> _vlists;
		return &_vlists;
	}

	volatile SDDAllocator* sdd_allocator()
	{
		// number of SDD (8B) objects available in user sketch
		static volatile SDDAllocatorT<64> _sdds;
		return &_sdds;
	}
}

namespace D
{
	volatile NodeAllocator* nodes = dependency::node_allocator();
	volatile VListAllocator* vlists = dependency::vlist_allocator();
	volatile SDDAllocator* sdds = dependency::sdd_allocator();
};
