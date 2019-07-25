#pragma once

#include <structures/specialized/vlist.h>
#include <structures/tuple.h>
#include <memory_management/specialized/tuple_allocator.h>

#include "../../def.h"

class Thread;

class sleep final
{
	friend class Thread;

	private: static volatile NodeAllocator* _nodes()
	{
		static volatile VNodeAllocator<16> _allocator;
		return &_allocator;
	}
	private: static volatile ITupleAllocator<Thread*, Time>* _tuples()
	{
		static volatile TupleAllocator<16, Thread*, Time> _allocator;
		return &_allocator;
	}
	
	private: sleep(const sleep&) = delete;
	private: sleep(sleep&&) = delete;
	private: sleep& operator=(const sleep&) = delete;
	private: sleep& operator=(sleep&&) = delete;
	
	private: volatile vlist<Tuple<Thread*, Time>> sleeping;
	private: volatile Time ticks;
	
	private: sleep() : sleeping(_nodes()), ticks(0) { }
	
	public: unsigned int size() volatile const { return sleeping.size(); }
	
	public: void clear() volatile
	{
		auto end = sleeping.end();

		for (auto i = sleeping.begin(); i != end; ++i)
		{
			_tuples()->dealloc(*i);
		}

		sleeping.clear();
	}
	
	public: void insert(volatile const Thread* thread, Time time) volatile
	{
		if (time == 0) return;
		time += ticks;

		auto i = sleeping.begin();
		auto end = sleeping.end();

		auto prev = end;

		for (; i != end; prev = i, ++i)
		{
			//Time& time = i->e<1>();
			Time& current = i->TupleLeaf<1, Time>::value;
			if (time < current) break;
		}

		auto tuple = _tuples()->alloc(const_cast<Thread*>(thread), time);

		if (prev == end) sleeping.push_front(tuple);
		else prev.insertAfter(tuple);
	}
	public: Thread* take() volatile
	{
		if (sleeping.size() == 0) return nullptr;

		auto first = sleeping.peek_front();

		//Time& time = first->e<1>();
		Time& time = first->TupleLeaf<1, Time>::value;

		if (ticks < time) return nullptr;

		//Thread* awaken = first->e<0>();
		Thread* awaken = first->TupleLeaf<0, Thread*>::value;
		
		sleeping.remove_front();
		_tuples()->dealloc(first);

		return awaken;
	}
	public: void tick() volatile
	{
		++ticks; // (im)possible overflow?
		//if (sleeping.size() == 0) ticks = 0; // possible solution? - this could be unsynchronized code?

		// all of these assumptions are highly questionable and debatable
	}
	
	public: vlist_enumerator<Tuple<Thread*, Time>> begin() volatile { return sleeping.begin(); }
	public: vlist_enumerator<Tuple<Thread*, Time>> end() volatile { return sleeping.end(); }
	
	public: const vlist_enumerator<Tuple<Thread*, Time>> cbegin() volatile const { return sleeping.cbegin(); }
	public: const vlist_enumerator<Tuple<Thread*, Time>> cend() volatile const { return sleeping.cend(); }
};
