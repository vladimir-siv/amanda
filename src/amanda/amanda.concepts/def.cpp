#include "structures/specialized/vlist.h"

vlist_allocator::Node vlist_allocator::nodes[_VLIST_ALLOCATOR_SIZE_];
bitvector<_VLIST_ALLOCATOR_SIZE_> vlist_allocator::usage;
unsigned int vlist_allocator::ptr = _VLIST_ALLOCATOR_SIZE_ - 1;
