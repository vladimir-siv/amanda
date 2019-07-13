#include "structures/specialized/vlist.h"
#include "structures/specialized/vmultilist.h"

volatile NodeAllocator NodeAllocator::_default;

//volatile NodeAllocator vsublist_allocator::_nodes;
volatile TupleAllocator<vlist<void>*, unsigned long long> vsublist_allocator::_tuples;
volatile ReinterpretingAllocator<vlist<void>, unsigned int, void*, void*, volatile void*> vsublist_allocator::_vlists;
