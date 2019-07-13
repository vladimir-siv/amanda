#pragma once

#include "vlist.h"

#include "../tuple.h"
#include "../../memory_management/specialized/tuple_allocator.h"
#include "../../memory_management/specialized/reinterpreting_allocator.h"

template <typename T> using vsublist = Tuple<vlist<T>*, unsigned long long>;

class vsublist_allocator
{
	template <typename T> friend class vmultilist;

	//private: static volatile NodeAllocator _nodes;
	private: static volatile TupleAllocator<vlist<void>*, unsigned long long> _tuples;
	private: static volatile ReinterpretingAllocator<vlist<void>, unsigned int, void*, void*, volatile void*> _vlists;
	
	private: static inline Tuple<vlist<void>*, unsigned long long>* alloc(unsigned long long tag = 0)
	{
		//auto vlist = _vlists.alloc(0, nullptr, nullptr, &_nodes);
		auto vlist = _vlists.alloc(0, nullptr, nullptr, nullptr);
		return _tuples.alloc(vlist, tag);
	}
	private: static inline void dealloc(Tuple<vlist<void>*, unsigned long long>* object)
	{
		if (object == nullptr) return;
		_vlists.dealloc(object->e<0>());
		_tuples.dealloc(object);
	}
	
	private: template <typename T> static inline vsublist<T>* alloc(unsigned long long tag = 0)
	{
		xassert(sizeof(vlist<T>) == sizeof(vlist<void>));
		auto object = reinterpret_cast<vsublist<T>*>(alloc(tag));
		//object->e<0>()->clear();
		object->TupleLeaf<0, vlist<T>*>::value->clear();
		return object;
	}
	private: template <typename T> static inline void dealloc(vsublist<T>* object)
	{
		xassert(sizeof(vlist<T>) == sizeof(vlist<void>));
		//object->e<0>()->clear();
		object->TupleLeaf<0, vlist<T>*>::value->clear();
		dealloc(reinterpret_cast<Tuple<vlist<void>*, unsigned long long>*>(object));
		return;
	}
};

template <typename T>
class vmultilist final
{
	public: static unsigned int tuples_available() { return vsublist_allocator::_tuples.available(); }
	public: static unsigned int vlists_available() { return vsublist_allocator::_vlists.available(); }
	
	private: volatile vlist<vsublist<T>> list;
	private: volatile unsigned long long ticks = 0;
	
	//public: vmultilist() : list(&vsublist_allocator::_nodes) { }
	
	public: unsigned int size() volatile const { return list.size(); }
	
	public: void clear() volatile
	{
		auto end = list.end();
		
		for (auto i = list.begin(); i != end; ++i)
		{
			vsublist_allocator::dealloc<T>(*i);
		}

		list.clear();
	}
	
	public: void insert(const T* object, unsigned long long tag) volatile
	{
		if (tag == 0) return;
		tag += ticks;

		auto i = list.begin();
		auto end = list.end();

		auto prev = end;

		for (; i != end; prev = i, ++i)
		{
			// unsigned long long& ctag = i->e<1>();
			unsigned long long& ctag = i->TupleLeaf<1, unsigned long long>::value;
			
			if (ctag == tag)
			{
				// i->e<0>()->push_back(object);
				i->TupleLeaf<0, vlist<T>*>::value->push_back(object);
				return;
			}

			if (ctag > tag) break;
		}

		vsublist<T>* sublist = vsublist_allocator::alloc<T>(tag);
		//sublist->e<0>()->push_back(object);
		sublist->TupleLeaf<0, vlist<T>*>::value->push_back(object);
		
		if (prev == end) list.push_front(sublist);
		else prev.insertAfter(sublist);
	}
	public: T* take() volatile
	{
		if (list.size() == 0) return nullptr;
		
		vsublist<T>* first = list.peek_front();
		
		//unsigned long long& ctag = first->e<1>();
		unsigned long long& ctag = first->TupleLeaf<1, unsigned long long>::value;

		if (ticks < ctag) return nullptr;
		
		//vlist<T>* flist = first->e<0>();
		vlist<T>* flist = first->TupleLeaf<0, vlist<T>*>::value;
		T* value = flist->pop_front();

		if (flist->size() == 0)
		{
			list.remove_front();
			vsublist_allocator::dealloc(first);
		}

		return value;
	}
	public: void tick() volatile
	{
		++ticks; // (im)possible overflow?
		//if (list.size() == 0) ticks = 0; // possible solution? - this could be unsynchronized code?
		
		// all of these assumptions are highly questionable and debatable
	}
	
	public: vlist_enumerator<vsublist<T>> begin() volatile { return list.begin(); }
	public: vlist_enumerator<vsublist<T>> end() volatile { return list.end(); }
	
	public: const vlist_enumerator<vsublist<T>> cbegin() volatile const { return list.cbegin(); }
	public: const vlist_enumerator<vsublist<T>> cend() volatile const { return list.cend(); }
};
