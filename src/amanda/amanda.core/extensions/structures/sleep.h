#pragma once

#include <structures/specialized/vlist.h>
#include <structures/tuple.h>
#include <memory_management/specialized/tuple_allocator.h>

#include "../../def.h"
#include "../../thread.h"

class sleep final
{
	friend class Thread;

	private: static volatile NodeAllocator* _nodes()
	{
		static volatile VNodeAllocator<16> _allocator;
		return &_allocator;
	}
	
	public: static volatile sleep& instance() { static sleep i; return i; }
	
	private: sleep(const sleep&) = delete;
	private: sleep(sleep&&) = delete;
	private: sleep& operator=(const sleep&) = delete;
	private: sleep& operator=(sleep&&) = delete;
	
	private: volatile vlist<Thread> sleeping;
	private: volatile Time ticks;
	
	private: sleep() : sleeping(_nodes()), ticks(0) { }
	
	public: unsigned int size() volatile const { return sleeping.size(); }
	
	public: void clear() volatile
	{
		sleeping.clear();
	}
	
	public: void insert(volatile Thread* thread, Time time) volatile
	{
		if (time == 0) return;
		time += ticks;

		auto i = sleeping.begin();
		auto end = sleeping.end();

		auto prev = end;

		for (; i != end; prev = i, ++i)
		{
			if (time < i->point) break;
		}

		thread->point = time;

		if (prev == end) sleeping.push_front((const Thread*)thread);
		else prev.insertAfter((const Thread*)thread);
	}
	public: Thread* take() volatile
	{
		if (sleeping.size() == 0) return nullptr;

		auto first = sleeping.peek_front();

		if (ticks < first->point) return nullptr;
		
		sleeping.remove_front();

		return first;
	}
	public: void tick() volatile
	{
		++ticks; // (im)possible overflow?
		//if (sleeping.size() == 0) ticks = 0; // possible solution? - this could be unsynchronized code?

		// all of these assumptions are highly questionable and debatable
	}
	
	public: vlist_enumerator<Thread> begin() volatile { return sleeping.begin(); }
	public: vlist_enumerator<Thread> end() volatile { return sleeping.end(); }
	
	public: const vlist_enumerator<Thread> cbegin() volatile const { return sleeping.cbegin(); }
	public: const vlist_enumerator<Thread> cend() volatile const { return sleeping.cend(); }
};
