#pragma once

#include "../types/type_traits.h"
#include "../exceptions.h"

template <typename T>
class queue final
{
	private: unsigned int _capacity;
	private: unsigned int _size = 0;
	private: unsigned int _first = 0, _last = 0;
	private: T* _data = nullptr;
	
	private: void copy(const queue& queue)
	{
		this->_capacity = queue._capacity;
		this->_size = queue._size;
		this->_first = queue._first;
		this->_last = queue._last;
		this->_data = new T[queue._capacity];
		for (unsigned int i = 0; i < queue._capacity; ++i)
			this->_data[i] = queue._data[i];
	}
	private: void move(queue& queue)
	{
		this->_capacity = queue._capacity;
		this->_size = queue._size;
		this->_first = queue._first;
		this->_last = queue._last;
		this->_data = queue._data;
		queue._data = nullptr;
	}
	private: void clean()
	{
		delete[] _data;
		_data = nullptr;

		_capacity = 0;
		_size = 0;
		_first = _last = 0;
	}
	
	public: explicit queue(unsigned int capacity) : _capacity(capacity), _data(new T[capacity]) { }
	public: queue(const queue& queue) : _capacity(0) { copy(queue); }
	public: queue(queue&& queue) { move(queue); }
	public: ~queue() { clean(); }
	
	public: const T& operator[](unsigned int i) const { return const_cast<queue&>(*this)[i]; }
	public: T& operator[](unsigned int i)
	{
		if (i < 0 || _capacity <= i)
		{
			Exceptions::Throw<IndexOutOfBoundsException>();
			return _data[0]; // dummy
		}

		return _data[i];
	}
	
	public: queue& operator=(const queue& queue)
	{
		if (this != &queue) { clean(); copy(queue); }
		return *this;
	}
	public: queue& operator=(queue&& queue)
	{
		if (this != &queue) { clean(); move(queue); }
		return *this;
	}
	
	public: unsigned int capacity() const { return _capacity; }
	public: unsigned int size() const { return _size; }
	
	public: void clear()
	{
		_size = 0;
		_first = _last = 0;
	}
	
	public: void enqueue(const T& value)
	{
		if (_size >= _capacity)
		{
			Exceptions::Throw<CollectionFullException>();
			return;
		}

		_data[_last] = value;
		if (++_last >= _capacity) _last = 0;
		++_size;
	}
	public: void enqueue(T&& value)
	{
		if (_size >= _capacity)
		{
			Exceptions::Throw<CollectionFullException>();
			return;
		}

		_data[_last] = std::move(value);
		if (++_last >= _capacity) _last = 0;
		++_size;
	}
	public: T& dequeue()
	{
		if (_size <= 0)
		{
			Exceptions::Throw<CollectionEmptyException>();
			return _data[0]; // dummy
		}

		T& value = _data[_first];
		if (++_first >= _capacity) _first = 0;
		--_size;
		return value;
	}
	public: T& peek() const
	{
		if (_size <= 0)
		{
			Exceptions::Throw<CollectionEmptyException>();
			return _data[0]; // dummy
		}

		return _data[_first];
	}
};
