#pragma once

#include <types/def.h>
#include <types/type_traits.h>
#include <patterns/enumerator.h>
#include <rtti/type.h>

#include "../../dependency.h"

class sdd_assume;

template <typename T>
class sdd_list_t;

template <typename T>
class sdd_list_enumerator;

template <typename T>
struct sdd_list final // this is actually a representation of a node in a list!
{
	friend class sdd_list_t<T>;
	friend class sdd_list_enumerator<T>;

	private: sdd::type<T*> value;
	private: sdd::type<sdd_list*> next;
	
	public: static sdd_list* _new()
	{
		// Okay I know this looks VERY wierd but this is how this actaully
		// REALLY works. THIS is valid way to allocate a new list. How,
		// and more importantly, why? Well, first of all, this isn't an
		// allocation of A LIST, but of A NODE in the list. That's why this
		// is just a convenience function to look like something is being
		// allocated, but the pointer itself to the first element (node)
		// of the list represents THE REAL list (the object list). Wierd,
		// yes, but think about it and you'll see it makes sense. THINK!
		return nullptr;
	}
	private: static sdd_list* _new(const T* value, sdd_list* next = nullptr)
	{
		return D::sdds->alloc<sdd_list>(sdd::cast((T*)value), sdd::cast(next));
	}
	private: sdd_list(T* value, sdd_list* next = nullptr) :
		value(value),
		next(next)
	{ }
};

template <typename T>
class sdd_list_t final
{
	friend class sdd_list_enumerator<T>;
	friend class sdd_assume;

	using Node = sdd_list<T>;
	
	private: Node*& _first;
	
	private: sdd_list_t(sdd_list<T>*& lst) : _first(lst) { }
	
	public: void clear()
	{
		for (Node* i = _first, *temp; i != nullptr; )
		{
			temp = i;
			i = i->next.real;
			//delete temp;
			D::sdds->dealloc(temp);
		}

		_first = nullptr;
	}
	
	public: void push(const T* value)
	{
		_first = Node::_new(value, _first);
	}
	public: T* peek()
	{
		if (_first == nullptr) return nullptr;
		return _first->value.real;
	}
	public: T* pop()
	{
		if (_first == nullptr) return nullptr;
		Node* temp = _first;
		_first = _first->next.real;
		T* value = temp->value.real;
		D::sdds->dealloc(temp);
		return value;
	}
	
	public: sdd_list_enumerator<T> begin() { return sdd_list_enumerator<T>(this, _first); }
	public: sdd_list_enumerator<T> end() { return sdd_list_enumerator<T>(this, nullptr); }
	
	public: const sdd_list_enumerator<T> cbegin() const { return sdd_list_enumerator<T>(const_cast<sdd_list_t<T>*>(this), _first); }
	public: const sdd_list_enumerator<T> cend() const { return sdd_list_enumerator<T>(const_cast<sdd_list_t<T>*>(this), nullptr); }
};

template <typename T>
class sdd_list_enumerator : public enumerator<T*>
{
	__RTTI__
	
	friend class sdd_list_t<T>;
	using Node = typename sdd_list_t<T>::Node;

	protected: sdd_list_t<T>* lst;
	protected: Node* ptr;
	protected: sdd_list_enumerator(sdd_list_t<T>* lst, Node* ptr) : lst(lst), ptr(ptr) { }
	
	public: virtual ~sdd_list_enumerator() { }
	public: virtual sdd_list_enumerator* clone() const override { return new sdd_list_enumerator(lst, ptr); }
	
	public: virtual T*& operator*() const override { return ptr->value.real; }
	public: virtual T*& operator->() const override { return ptr->value.real; }
	
	public: virtual sdd_list_enumerator<T>& operator++() override
	{
		if (ptr != nullptr) ptr = ptr->next.real;
		return *this;
	}
	public: virtual sdd_list_enumerator<T> operator++(int)
	{
		sdd_list_enumerator clone(lst, ptr);
		++(*this);
		return clone;
	}
	
	public: virtual bool equals(const enumerator<T*>& other) const override
	{
		if (typeid(*this) != typeid(other)) return false;
		const sdd_list_enumerator<T>& oth = (const sdd_list_enumerator<T>&)other;
		return lst == oth.lst && ptr == oth.ptr;
	}
	
	public: virtual void begin()
	{
		ptr = lst->_first;
	}
	
	public: virtual bool insertAfter(const T* value)
	{
		if (ptr == nullptr) return false;

		//ptr->next = new Node(value, ptr->next);
		ptr->next.real = Node::_new(value, ptr->next.real);

		return true;
	}
	public: virtual bool removeNext()
	{
		if (ptr == nullptr || ptr->next.real == nullptr) return false;

		Node* removing = ptr->next.real;
		ptr->next.real = removing->next.real;
		//delete removing;
		D::sdds->dealloc(removing);
		
		return true;
	}
	
	public: virtual T* next() const { return ptr->next.real->value.real; }
};

class sdd_assume final
{
	private: sdd_assume() = delete;
	private: ~sdd_assume() = delete;
	
	public: template <typename T> static sdd_list_t<T> list(sdd_list<T>*& lst)
	{
		sdd_list_t<T> t((sdd_list<T>*&)lst);
		return t;
	}
	public: template <typename T> static sdd_list_t<T> list(sdd_list<T>* const& lst)
	{
		sdd_list_t<T> t((sdd_list<T>*&)lst);
		return t;
	}
};
