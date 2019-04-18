#include "structures/specialized/vlist.h"
#include "structures/specialized/vmultilist.h"

vlist_allocator::Node vlist_allocator::nodes[_VLIST_ALLOCATOR_SIZE_];
bitvector<_VLIST_ALLOCATOR_SIZE_> vlist_allocator::usage;
unsigned int vlist_allocator::ptr = _VLIST_ALLOCATOR_SIZE_ - 1;

TupleAllocator<vlist<void>*, unsigned long long> vsublist_allocator::_tuples;
ReinterpretingAllocator<vlist<void>, void*, void*, unsigned int> vsublist_allocator::_vlists;
