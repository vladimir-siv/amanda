#pragma once

#include "../types/type_traits.h"
#include "../event_system/delegate.h"
#include "../exceptions.h"
#include "../patterns/enumerator.h"

template <typename T> class ListEnumerator;

template <typename T>
class List final
{
	friend class ListEnumerator<T>;

	private: struct Node final
	{
		T value;
		Node* next;
		Node(const T& value, Node* next = nullptr) : value(value), next(next) { }
		Node(T&& value, Node* next = nullptr) : value(std::forward<T>(value)), next(next) { }
	};
	
	private: static ListEnumerator<T> _end;
	
	private: unsigned int _size = 0;
	private: Node* _first = nullptr; Node* _last = nullptr;
	
	private: void copy(const List& list)
	{
		for (Node* i = list._first; i != nullptr; i = i->next)
		{
			push_back(i->value);
		}
	}
	private: void move(List& list)
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
			delete temp;
		}

		_size = 0;
		_first = _last = nullptr;
	}
	
	public: explicit List() { }
	public: List(const List& list) { copy(list); }
	public: List(List&& list) { move(list); }
	public: ~List() { clean(); }
	
	public: List& operator=(const List& list)
	{
		if (this != &list) { clean(); copy(list); }
		return *this;
	}
	public: List& operator=(List&& list)
	{
		if (this != &list) { clean(); move(list); }
		return *this;
	}
	
	public: unsigned int size() const { return _size; }
	
	public: void clear() { clean(); }
	
	public: void push_front(const T& value)
	{
		_first = new Node(value, _first);
		if (_size++ == 0) _last = _first;
	}
	public: void push_front(T&& value)
	{
		_first = new Node(std::forward<T>(value), _first);
		if (_size++ == 0) _last = _first;
	}
	public: T pop_front()
	{
		if (_size == 0)
		{
			Exceptions::Throw<CollectionEmptyException>();
			return _first->value;
		}

		Node* temp = _first;
		_first = _first->next;
		if (--_size == 0) _last = nullptr;
		T value = std::move(temp->value);
		delete temp;
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
		delete temp;
	}
	public: T& peek_front() const
	{
		return _first->value;
	}
	public: void push_back(const T& value)
	{
		Node* node = new Node(value);
		if (_size == 0) _first = _last = node;
		else _last = _last->next = node;
		++_size;
	}
	public: void push_back(T&& value)
	{
		Node* node = new Node(std::forward<T>(value));
		if (_size == 0) _first = _last = node;
		else _last = _last->next = node;
		++_size;
	}
	public: T& peek_back() const
	{
		return _last->value;
	}
	
	public: void remove_last(const T&& value) { remove_last(value); }
	public: void remove_last(const T& value)
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
				delete current;
			}
			else remove_front();
		}
	}
	public: void traverse(IDelegate<void, T&>* del)
	{
		for (Node* i = _first; i != nullptr; i = i->next)
		{
			del->invoke(i->value);
		}
	}
	
	public: ListEnumerator<T> begin() const { return ListEnumerator<T>(_first); }
	public: ListEnumerator<T>& end() const { return _end; }
};

template <typename T>
class ListEnumerator final : public Enumerator<T>
{
	friend class List<T>;

	private: typename List<T>::Node* ptr;
	private: ListEnumerator(typename List<T>::Node* ptr) : ptr(ptr) { }
	
	public: virtual T& operator*() const override { return ptr->value; }
	public: virtual T& operator->() const override { return ptr->value; }
	
	public: virtual ListEnumerator<T>& operator++() override
	{
		if (ptr != nullptr) ptr = ptr->next;
		return *this;
	}
	public: virtual ListEnumerator<T> operator++(int)
	{
		ListEnumerator clone(ptr);
		++(*this);
		return clone;
	}
	
	public: virtual bool operator!=(const Enumerator<T>& other) const override { return !(*this == other); }
	public: virtual bool operator==(const Enumerator<T>& other) const override
	{
		const ListEnumerator* oth = dynamic_cast<const ListEnumerator*>(&other);
		if (oth == nullptr) return false;
		return ptr == oth->ptr;
	}
	
	public: virtual ~ListEnumerator() override { }
	public: virtual ListEnumerator* clone() const override { return new ListEnumerator(ptr); }
};

template <typename T> ListEnumerator<T> List<T>::_end(nullptr);
