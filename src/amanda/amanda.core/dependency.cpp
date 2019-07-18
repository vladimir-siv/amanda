#include <Arduino.h>

#include "dependency.h"

namespace dependency
{
	volatile NodeAllocator* node_allocator()
	{
		// number of nodes available in user sketch
		static volatile TNodeAllocator<256> _nodes;
		return &_nodes;
	}
}
