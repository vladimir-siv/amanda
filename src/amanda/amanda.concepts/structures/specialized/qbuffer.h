#pragma once

template <typename T>
class QBuffer final
{
	private: unsigned int _capacity;
	private: unsigned int _size = 0;
	private: unsigned int _top = 0;
	private: T* _data = nullptr;
	
	private: void copy(const QBuffer& buffer)
	{
		this->_capacity = buffer._capacity;
		this->_size = buffer._size;
		this->_top = buffer._top;
		this->_data = new T[buffer._capacity];
		for (unsigned int i = 0; i < buffer._capacity; ++i)
			this->_data[i] = buffer._data[i];
	}
	private: void move(QBuffer& buffer)
	{
		this->_capacity = buffer._capacity;
		this->_size = buffer._size;
		this->_top = buffer._top;
		this->_data = buffer._data;
		buffer._data = nullptr;
	}
	private: void clean()
	{
		delete[] _data;
		_data = nullptr;

		_capacity = 0;
		_size = 0;
		_top = 0;
	}
	
	public: explicit QBuffer(unsigned int capacity) : _capacity(capacity), _data(new T[capacity]) { }
	public: QBuffer(const QBuffer& buffer) : _capacity(0) { copy(buffer); }
	public: QBuffer(QBuffer&& buffer) { move(buffer); }
	public: ~QBuffer() { clean(); }
	
	public: QBuffer& operator=(const QBuffer& buffer)
	{
		if (this != &buffer) { clean(); copy(buffer); }
		return *this;
	}
	public: QBuffer& operator=(QBuffer&& buffer)
	{
		if (this != &buffer) { clean(); move(buffer); }
		return *this;
	}
	
	public: unsigned int capacity() const { return _capacity; }
	public: unsigned int size() const { return _size; }
	
	public: void clear()
	{
		_size = _top = 0;
	}
	
	public: void push(T _value)
	{
		_data[_top] = _value;
		if (++_top == _capacity) _top = 0;
		if (++_size > _capacity) _size = _capacity;
	}
	public: T pop()
	{
		if (_size > 0)
		{
			--_size;
			if (_top-- == 0) _top = _capacity - 1;
		}

		return _data[_top];
	}
	public: T peek() const
	{
		return _data[_top == 0 ? _capacity - 1 : _top - 1];
	}
};
