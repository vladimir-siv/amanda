#include "structures/specialized/vlist.h"
#include "structures/specialized/vmultilist.h"

volatile NodeAllocator* NodeAllocator::_default()
{
	// number of nodes available in system
	static volatile VNodeAllocator<128> _default_nodes;
	return &_default_nodes;
}

volatile ITupleAllocator<vlist<void>*, unsigned long long>* vsublist_allocator::_tuples()
{
	// number of tuples available for vsublists
#ifdef __AMANDA_TESTS__
	static volatile TupleAllocator<64, vlist<void>*, unsigned long long> _tuple_space;
	return &_tuple_space;
#else
	return nullptr;
#endif
}
volatile IReinterpretingAllocator<vlist<void>, unsigned int, void*, void*, volatile void*>* vsublist_allocator::_vlists()
{
	// number of vlists available for vsublists
#ifdef __AMANDA_TESTS__
	static volatile ReinterpretingAllocator<64, vlist<void>, unsigned int, void*, void*, volatile void*> _vlist_space;
	return &_vlist_space;
#else
	return nullptr;
#endif
}
