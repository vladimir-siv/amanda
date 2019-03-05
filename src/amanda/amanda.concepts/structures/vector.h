#pragma once

#include "../types/type_traits.h"
#include "../exceptions.h"

template <typename T>
class Vector final
{
	private: unsigned int _capacity;
	private: unsigned int _size = 0;
	private: T* _data = nullptr;
	
	private: void copy(const Vector& vector)
	{
		this->_capacity = vector._capacity;
		this->_size = vector._size;
		this->_data = new T[vector._capacity];
		for (unsigned int i = 0; i < vector._capacity; ++i)
			this->_data[i] = vector._data[i];
	}
	private: void move(Vector& vector)
	{
		this->_capacity = vector._capacity;
		this->_size = vector._size;
		this->_data = vector._data;
		vector._data = nullptr;
	}
	private: void clean()
	{
		delete[] _data;
		_data = nullptr;

		_capacity = 0;
		_size = 0;
	}
	
	public: explicit Vector(unsigned int capacity) : _capacity(capacity), _data(new T[capacity]) { }
	public: Vector(const Vector& vector) : _capacity(0) { copy(vector); }
	public: Vector(Vector&& vector) { move(vector); }
	public: ~Vector() { clean(); }
	
	public: const T& operator[](unsigned int i) const { return const_cast<Vector&>(*this)[i]; }
	public: T& operator[](unsigned int i)
	{
		if (i < 0 || _capacity <= i)
		{
			Exceptions::Throw<IndexOutOfBoundsException>();
			return _data[0]; // dummy
		}

		return _data[i];
	}
	
	public: Vector& operator=(const Vector& vector)
	{
		if (this != &vector) { clean(); copy(vector); }
		return *this;
	}
	public: Vector& operator=(Vector&& vector)
	{
		if (this != &vector) { clean(); move(vector); }
		return *this;
	}
	
	public: unsigned int capacity() const { return _capacity; }
	public: unsigned int size() const { return _size; }
	
	public: void clear()
	{
		_size = 0;
	}
	
	public: void push(const T& value)
	{
		if (_size >= _capacity)
		{
			Exceptions::Throw<CollectionFullException>();
			return;
		}

		_data[_size++] = value;
	}
	public: void push(T&& value)
	{
		if (_size >= _capacity)
		{
			Exceptions::Throw<CollectionFullException>();
			return;
		}

		_data[_size++] = std::move(value);
	}
	public: T& pop()
	{
		if (_size <= 0)
		{
			Exceptions::Throw<CollectionEmptyException>();
			return _data[0]; // dummy
		}

		return _data[--_size];
	}
	public: T& peek() const
	{
		if (_size <= 0)
		{
			Exceptions::Throw<CollectionEmptyException>();
			return _data[0]; // dummy
		}

		return _data[_size - 1];
	}
};
