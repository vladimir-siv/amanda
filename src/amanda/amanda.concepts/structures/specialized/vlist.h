#pragma once

#include "../../assert/static.h"
#include "../../types/def.h"
#include "../../types/type_traits.h"
#include "../../event_system/delegate.h"
#include "../../exceptions.h"
#include "../../patterns/enumerator.h"
#include "../../memory_management/object_allocator.h"

#ifndef __USE_RTTI__
#include "../../rtti/type.h"
#endif

class NodeAllocator
{
	template <typename T> friend class vlist;
	template <typename T> friend class vlist_enumerator;

	public: static volatile NodeAllocator* _default();
	
	protected: struct Node final
	{
		void* info;
		Node* next;
	};
	
	public: virtual ~NodeAllocator() { }
	
	protected: virtual Node* _alloc() volatile = 0;
	protected: virtual void _dealloc(Node* object) volatile = 0;
	
	private: template <typename T> T alloc() volatile
	{
		xassert(sizeof(std::remove_pointer_t<T>) == sizeof(Node));
		return (T)(_alloc());
	}
	private: template <typename T> void dealloc(T object) volatile
	{
		xassert(sizeof(std::remove_pointer_t<T>) == sizeof(Node));
		_dealloc((Node*)(object));
	}
};

template <size_t size>
class VNodeAllocator : public NodeAllocator
{
	private: volatile ObjectAllocator<NodeAllocator::Node, size> _memory;
	
	public: virtual ~VNodeAllocator() { }
	
	protected: virtual NodeAllocator::Node* _alloc() volatile override { return _memory.alloc(); }
	protected: virtual void _dealloc(NodeAllocator::Node* object) volatile override { _memory.dealloc(object); }
};

template <typename T> class vlist_enumerator;

template <typename T>
class vlist final
{
	friend class vlist_enumerator<T>;

	private: struct Node final
	{
		T* value;
		volatile Node* next;
		volatile Node* construct(const T* value, volatile Node* next = nullptr) volatile
		{
			this->value = const_cast<T*>(value);
			this->next = next;
			return this;
		}
	};
	
	private: volatile unsigned int _size = 0;
	private: volatile Node* _first = nullptr; volatile Node* _last = nullptr;
	private: volatile NodeAllocator* _allocator = nullptr;
	
	private: void copy(const vlist& list) volatile
	{
		for (volatile Node* i = list._first; i != nullptr; i = i->next)
		{
			push_back(i->value);
		}
	}
	private: void move(vlist& list) volatile
	{
		if (this->_allocator == list._allocator)
		{
			this->_size = list._size;
			this->_first = list._first;
			this->_last = list._last;
			list._first = list._last = nullptr;
			list._size = 0;
		}
		else copy(list);
	}
	private: void clean() volatile
	{
		for (volatile Node* i = _first, *temp; i != nullptr; )
		{
			temp = i;
			i = i->next;
			//delete temp;
			_allocator->dealloc(temp);
		}

		_size = 0;
		_first = _last = nullptr;
	}
	
	public: explicit vlist(volatile NodeAllocator* allocator = NodeAllocator::_default()) : _allocator(allocator) { }
	public: vlist(const vlist& list) { copy(list); }
	public: vlist(vlist&& list) { move(list); }
	public: ~vlist() { clean(); }
	
	public: vlist& operator=(const vlist& list) volatile
	{
		if (this != &list) { clean(); copy(list); }
		return *this;
	}
	public: vlist& operator=(vlist&& list) volatile
	{
		if (this != &list) { clean(); move(list); }
		return *this;
	}
	
	public: unsigned int size() volatile const { return _size; }
	
	public: void clear() volatile { clean(); }
	
	public: void push_front(const T* value) volatile
	{
		//_first = new Node(value, _first);
		_first = _allocator->alloc<volatile Node*>()->construct(value, _first);
		if (_size++ == 0) _last = _first;
	}
	public: T* pop_front() volatile
	{
		if (_size == 0)
		{
			Exceptions::Throw<CollectionEmptyException>();
			return nullptr;
		}

		volatile Node* temp = _first;
		_first = _first->next;
		if (--_size == 0) _last = nullptr;
		T* value = std::move(temp->value);
		//delete temp;
		_allocator->dealloc(temp);
		return value;
	}
	public: void remove_front() volatile
	{
		if (_size == 0)
		{
			Exceptions::Throw<CollectionEmptyException>();
			return;
		}

		volatile Node* temp = _first;
		_first = _first->next;
		if (--_size == 0) _last = nullptr;
		//delete temp;
		_allocator->dealloc(temp);
	}
	public: T* peek_front() volatile const
	{
		return _first->value;
	}
	public: void push_back(const T* value) volatile
	{
		//Node* node = new Node(value);
		volatile Node* node = _allocator->alloc<volatile Node*>()->construct(value);
		if (_size == 0) _first = _last = node;
		else _last = _last->next = node;
		++_size;
	}
	public: T* peek_back() volatile const
	{
		return _last->value;
	}
	
	public: void remove_last(const T* value) volatile
	{
		volatile Node* before = nullptr;
		volatile Node* current = nullptr;

		for (volatile Node* i = _first, *prev = nullptr; i != nullptr; prev = i, i = i->next)
		{
			if (i->value == value)
			{
				before = prev;
				current = i;
			}
		}

		if (current != nullptr)
		{
			if (before != nullptr)
			{
				before->next = current->next;
				if (before->next == nullptr) _last = before;
				--_size;
				//delete current;
				_allocator->dealloc(current);
			}
			else remove_front();
		}
	}
	
	public: void traverse(IDelegate<void, T*>* del) volatile
	{
		for (volatile Node* i = _first; i != nullptr; i = i->next)
		{
			del->invoke(i->value);
		}
	}
	
	public: void merge(vlist& list) volatile
	{
		if (list._size == 0) return;
		
		if (_size > 0)
		{
			_last->next = list._first;
			_last = list._last;
			_size += list._size;

			list._first = list._last = nullptr;
			list._size = 0;
		}
		else move(list);
	}
	public: T* second() volatile const
	{
		if (_size < 2) return nullptr;
		return _first->next->value;
	}
	public: void remove_second() volatile
	{
		if (_size < 2) return;

		auto second = _first->next;
		
		if (_last == second) _last = _first;
		_first->next = second->next;
		--_size;

		// delete second
		_allocator->dealloc(second);
	}
	
	public: vlist_enumerator<T> begin() volatile { return vlist_enumerator<T>(this, _first); }
	public: vlist_enumerator<T> end() volatile { return vlist_enumerator<T>(this, nullptr); }
	
	public: const vlist_enumerator<T> cbegin() volatile const { return vlist_enumerator<T>(const_cast<volatile vlist<T>*>(this), _first); }
	public: const vlist_enumerator<T> cend() volatile const { return vlist_enumerator<T>(const_cast<volatile vlist<T>*>(this), nullptr); }
};

template <typename T>
class vlist_enumerator : public enumerator<T*>
{
	__RTTI__
	
	friend class vlist<T>;
	using Node = typename vlist<T>::Node;

	protected: volatile vlist<T>* lst;
	protected: volatile Node* ptr;
	protected: vlist_enumerator(volatile vlist<T>* lst, volatile Node* ptr) : lst(lst), ptr(ptr) { }
	
	public: virtual ~vlist_enumerator() override { }
	public: virtual vlist_enumerator* clone() const override { return new vlist_enumerator(lst, ptr); }
	
	public: virtual T*& operator*() const override { return const_cast<T*&>(ptr->value); }
	public: virtual T*& operator->() const override { return const_cast<T*&>(ptr->value); }
	
	public: virtual vlist_enumerator<T>& operator++() override
	{
		if (ptr != nullptr) ptr = ptr->next;
		return *this;
	}
	public: virtual vlist_enumerator<T> operator++(int)
	{
		vlist_enumerator clone(lst, ptr);
		++(*this);
		return clone;
	}
	
	public: virtual bool equals(const enumerator<T*>& other) const override
	{
		if (typeid(*this) != typeid(other)) return false;
		const vlist_enumerator<T>& oth = (const vlist_enumerator<T>&)other;
		return lst == oth.lst && ptr == oth.ptr;
	}
	
	public: virtual void begin()
	{
		ptr = lst->_first;
	}
	
	public: virtual bool insertAfter(const T* value)
	{
		if (ptr == nullptr) return false;

		//ptr->next = new typename vlist<T>::Node(value, ptr->next);
		ptr->next = lst->_allocator->template alloc<volatile Node*>()->construct(value, ptr->next);
		if (lst->_last == ptr) lst->_last = ptr->next;
		++lst->_size;

		return true;
	}
	
	public: virtual T* next() volatile const { return ptr->next->value; }
};
