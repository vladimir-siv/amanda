#pragma once

#include "../../types/def.h"
#include "../../types/type_traits.h"
#include "../../event_system/delegate.h"
#include "../../exceptions.h"
#include "../../patterns/enumerator.h"

#include "../bitvector.h"

#ifndef __USE_RTTI__
#include "../../rtti/type.h"
#endif

#define _VLIST_ALLOCATOR_SIZE_ 32

class vlist_allocator
{
	template <typename T> friend class vlist;
	template <typename T> friend class vlist_enumerator;
	
	private: struct Node final
	{
		void* info;
		Node* next;
	};
	
	private: static Node nodes[_VLIST_ALLOCATOR_SIZE_];
	private: static bitvector<_VLIST_ALLOCATOR_SIZE_> usage;
	private: static unsigned int ptr;
	
	private: static void* alloc()
	{
		for (unsigned int i = 0; i < _VLIST_ALLOCATOR_SIZE_; ++i)
		{
			if (++ptr == _VLIST_ALLOCATOR_SIZE_) ptr = 0;

			if (!usage.isset(ptr))
			{
				usage.set(ptr);
				return nodes + ptr;
			}
		}

		return nullptr;
	}
	private: static void dealloc(void* object)
	{
		usage.unset((Node*)object - nodes);
	}
	
	private: template <typename T> static inline T alloc() { return static_cast<T>(alloc()); }
	private: template <typename T> static inline void dealloc(T object) { dealloc(static_cast<void*>(object)); }
};

template <typename T> class vlist_enumerator;

template <typename T>
class vlist final
{
	friend class vlist_enumerator<T>;

	private: struct Node final
	{
		T* value;
		Node* next;
		Node* construct(const T* value, Node* next = nullptr)
		{
			this->value = const_cast<T*>(value);
			this->next = next;
			return this;
		}
	};
	
	private: unsigned int _size = 0;
	private: Node* _first = nullptr; Node* _last = nullptr;
	
	private: void copy(const vlist& list)
	{
		for (Node* i = list._first; i != nullptr; i = i->next)
		{
			push_back(i->value);
		}
	}
	private: void move(vlist& list)
	{
		this->_size = list._size;
		this->_first = list._first;
		this->_last = list._last;
		list._first = list._last = nullptr;
	}
	private: void clean()
	{
		for (Node* i = _first, *temp; i != nullptr; )
		{
			temp = i;
			i = i->next;
			//delete temp;
			vlist_allocator::dealloc(temp);
		}

		_size = 0;
		_first = _last = nullptr;
	}
	
	public: explicit vlist() { }
	public: vlist(const vlist& list) { copy(list); }
	public: vlist(vlist&& list) { move(list); }
	public: ~vlist() { clean(); }
	
	public: vlist& operator=(const vlist& list)
	{
		if (this != &list) { clean(); copy(list); }
		return *this;
	}
	public: vlist& operator=(vlist&& list)
	{
		if (this != &list) { clean(); move(list); }
		return *this;
	}
	
	public: unsigned int size() const { return _size; }
	
	public: void clear() { clean(); }
	
	public: void push_front(const T* value)
	{
		//_first = new Node(value, _first);
		_first = vlist_allocator::alloc<Node*>()->construct(value, _first);
		if (_size++ == 0) _last = _first;
	}
	public: T* pop_front()
	{
		if (_size == 0)
		{
			Exceptions::Throw<CollectionEmptyException>();
			return _first->value;
		}

		Node* temp = _first;
		_first = _first->next;
		if (--_size == 0) _last = nullptr;
		T* value = std::move(temp->value);
		//delete temp;
		vlist_allocator::dealloc(temp);
		return value;
	}
	public: void remove_front()
	{
		if (_size == 0)
		{
			Exceptions::Throw<CollectionEmptyException>();
			return;
		}

		Node* temp = _first;
		_first = _first->next;
		if (--_size == 0) _last = nullptr;
		//delete temp;
		vlist_allocator::dealloc(temp);
	}
	public: T* peek_front() const
	{
		return _first->value;
	}
	public: void push_back(const T* value)
	{
		//Node* node = new Node(value);
		Node* node = vlist_allocator::alloc<Node*>()->construct(value);
		if (_size == 0) _first = _last = node;
		else _last = _last->next = node;
		++_size;
	}
	public: T* peek_back() const
	{
		return _last->value;
	}

	public: void remove_last(const T* value)
	{
		Node* before = nullptr;
		Node* current = nullptr;

		for (Node* i = _first, *prev = nullptr; i != nullptr; prev = i, i = i->next)
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
				vlist_allocator::dealloc(current);
			}
			else remove_front();
		}
	}
	
	public: void traverse(IDelegate<void, T*>* del)
	{
		for (Node* i = _first; i != nullptr; i = i->next)
		{
			del->invoke(i->value);
		}
	}
	
	public: vlist_enumerator<T> begin() { return vlist_enumerator<T>(this, _first); }
	public: vlist_enumerator<T> end() { return vlist_enumerator<T>(this, nullptr); }
	
	public: const vlist_enumerator<T> cbegin() const { return vlist_enumerator<T>(const_cast<vlist<T>*>(this), _first); }
	public: const vlist_enumerator<T> cend() const { return vlist_enumerator<T>(const_cast<vlist<T>*>(this), nullptr); }
};

template <typename T>
class vlist_enumerator : public enumerator<T*>
{
	__RTTI__
	
	friend class vlist<T>;

	protected: vlist<T>* lst;
	protected: typename vlist<T>::Node* ptr;
	protected: vlist_enumerator(vlist<T>* lst, typename vlist<T>::Node* ptr) : lst(lst), ptr(ptr) { }
	
	public: virtual ~vlist_enumerator() override { }
	public: virtual vlist_enumerator* clone() const override { return new vlist_enumerator(lst, ptr); }
	
	public: virtual T*& operator*() const override { return ptr->value; }
	public: virtual T*& operator->() const override { return ptr->value; }
	
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
		ptr->next = vlist_allocator::alloc<typename vlist<T>::Node*>()->construct(value, ptr->next);
		if (lst->_last == ptr) lst->_last = ptr->next;
		++lst->_size;

		return true;
	}
	
	public: virtual T* next() const { return ptr->next->value; }
};
