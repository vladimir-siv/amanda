#include "structures/specialized/vlist.h"
#include "structures/specialized/vmultilist.h"

volatile NodeAllocator* NodeAllocator::_default()
{
	static volatile VNodeAllocator<128> _default_nodes; // number of nodes available in system
	return &_default_nodes;
}

volatile TupleAllocator<vlist<void>*, unsigned long long> vsublist_allocator::_tuples;
volatile ReinterpretingAllocator<vlist<void>, unsigned int, void*, void*, volatile void*> vsublist_allocator::_vlists;
