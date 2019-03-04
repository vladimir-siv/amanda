#pragma once

#include "../types/type_traits.h"
#include "../exceptions.h"

template <typename T>
class List final
{
	private: struct Node final
	{
		T value;
		Node* next;
		Node(const T& value, Node* next = nullptr) : value(value), next(next) { }
		Node(T&& value, Node* next = nullptr) : value(type_traits::forward<T>(value)), next(next) { }
	};
	
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
		_first = new Node(type_traits::forward<T>(value), _first);
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
		T value = type_traits::move(temp->value);
		delete temp;
		return value;
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
		Node* node = new Node(type_traits::forward<T>(value));
		if (_size == 0) _first = _last = node;
		else _last = _last->next = node;
		++_size;
	}
	public: T& peek_back() const
	{
		return _last->value;
	}
};
