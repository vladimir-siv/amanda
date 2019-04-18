#pragma once

#include "vlist.h"

#include "../tuple.h"
#include "../../memory_management/specialized/tuple_allocator.h"
#include "../../memory_management/specialized/reinterpreting_allocator.h"

#include "../../utility.h"

template <typename T> using vsublist = Tuple<vlist<T>*, unsigned long long>;

class vsublist_allocator
{
	template <typename T> friend class vmultilist;

	private: static TupleAllocator<vlist<void>*, unsigned long long> _tuples;
	private: static ReinterpretingAllocator<vlist<void>, void*, void*, unsigned int> _vlists;
	
	private: static inline Tuple<vlist<void>*, unsigned long long>* alloc(unsigned long long tag = 0)
	{
		auto vlist = _vlists.alloc(nullptr, nullptr, 0);
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
		return reinterpret_cast<vsublist<T>*>(alloc(tag));
	}
	private: template <typename T> static inline void dealloc(vsublist<T>* object)
	{
		xassert(sizeof(vlist<T>) == sizeof(vlist<void>));
		dealloc(reinterpret_cast<Tuple<vlist<void>*, unsigned long long>*>(object));
	}
};

template <typename T>
class vmultilist final
{
	public: static unsigned int tuples_available() { return vsublist_allocator::_tuples.available(); }
	public: static unsigned int vlists_available() { return vsublist_allocator::_vlists.available(); }
	
	private: vlist<vsublist<T>> list;
	private: unsigned long long ticks = 0;
	
	public: unsigned int size() const { return list.size(); }
	
	public: void clear()
	{
		auto end = list.end();
		
		for (auto i = list.begin(); i != end; ++i)
		{
			vsublist_allocator::dealloc<T>(*i);
		}

		list.clear();
	}
	
	public: void insert(const T* object, unsigned long long tag)
	{
		if (tag == 0) return;

		auto i = list.begin();
		auto end = list.end();
		
		auto prev = end;

		for (; i != end; prev = i, ++i)
		{
			unsigned long long& ctag = i->TupleLeaf<1, unsigned long long>::value;
			
			if (tag == ctag)
			{
				// i->e<0>()->push_back(object);
				i->TupleLeaf<0, vlist<T>*>::value->push_back(object);
				return;
			}

			if (tag < ctag)
			{
				ctag -= tag;
				break;
			}

			tag -= ctag;
		}

		vsublist<T>* sublist = vsublist_allocator::alloc<T>(tag);
		//sublist->e<0>()->push_back(object);
		sublist->TupleLeaf<0, vlist<T>*>::value->push_back(object);
		
		if (prev == end) list.push_front(sublist);
		else prev.insertAfter(sublist);
	}
	public: T* take()
	{
		if (list.size() == 0) return nullptr;

		vsublist<T>* first = list.peek_front();
		
		//unsigned long long& ctag = first->e<1>();
		unsigned long long& ctag = first->TupleLeaf<1, unsigned long long>::value;

		if (ctag > 0) return nullptr;

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
	public: void tick(bool mark_only = false)
	{
		if (list.size() == 0) return;

		++ticks;
		if (mark_only) return;

		vsublist<T>* first = list.peek_front();
		vsublist<T>* second = list.second();

		while (ticks > 0)
		{
			//unsigned long long& ctag1 = first->e<1>();
			unsigned long long& ctag1 = first->TupleLeaf<1, unsigned long long>::value;

			if (ctag1 > 0)
			{
				unsigned long long dif = math::vmin(ctag1, ticks);
				ctag1 -= dif;
				ticks -= dif;
			}
			else if (second != nullptr)
			{
				//unsigned long long& ctag2 = second->e<1>();
				unsigned long long& ctag2 = second->TupleLeaf<1, unsigned long long>::value;

				unsigned long long dif = math::vmin(ctag2, ticks);
				ctag2 -= dif;
				ticks -= dif;

				if (ctag2 == 0)
				{
					//first->e<0>()->merge(*(second->e<0>()));
					first->TupleLeaf<0, vlist<T>*>::value->merge(*(second->TupleLeaf<0, vlist<T>*>::value));
					list.remove_second();
					vsublist_allocator::dealloc(second);
					second = list.second();
				}
			}
			else ticks = 0;
		}
	}
	
	public: inline vlist_enumerator<vsublist<T>> begin() { return list.begin(); }
	public: inline vlist_enumerator<vsublist<T>> end() { return list.end(); }
	
	public: inline const vlist_enumerator<vsublist<T>> cbegin() const { return list.cbegin(); }
	public: inline const vlist_enumerator<vsublist<T>> cend() const { return list.cend(); }
};
