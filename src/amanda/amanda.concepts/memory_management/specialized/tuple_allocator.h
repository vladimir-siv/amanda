#pragma once

#include "../../types/def.h"
#include "../../types/type_traits.h"
#include "../../structures/tuple.h"
#include "../object_allocator.h"

template <typename... Types>
class ITupleAllocator
{
	public: virtual ~ITupleAllocator() { }
	
	public: virtual unsigned int available() volatile const = 0;
	protected: virtual Tuple<Types...>* _alloc() volatile = 0;
	protected: virtual void _dealloc(Tuple<Types...>* tuple) volatile = 0;
	
	private: template <position_t i, typename Arg> static inline void _assign(Tuple<Types...>* tuple, Arg&& arg)
	{
		tuple->TupleLeaf<i, variadic_arg<i, Types...>>::value = arg;
	}
	private: template <position_t i, typename Arg, typename... Args> static inline void _assign(Tuple<Types...>* tuple, Arg&& arg, Args&&... args)
	{
		tuple->TupleLeaf<i, variadic_arg<i, Types...>>::value = arg;
		_assign<i + 1>(tuple, std::forward<Args>(args)...);
	}
	
	// Caution: tuple has undefined values
	public: Tuple<Types...>* alloc() volatile
	{
		auto tuple = _alloc();
		return tuple;
	}
	public: template <typename... Args> Tuple<Types...>* alloc(Args&&... arg) volatile
	{
		auto tuple = _alloc();
		_assign<0>(tuple, std::forward<Args>(arg)...);
		return tuple;
	}
	public: void dealloc(Tuple<Types...>* tuple) volatile
	{
		_dealloc(tuple);
	}
};

template <size_t size, typename... Types>
class TupleAllocator : public ITupleAllocator<Types...>
{
	protected: volatile ObjectAllocator<Tuple<Types...>, size> _tuplespace;
	
	public: virtual unsigned int available() volatile const override { return _tuplespace.available(); }
	
	protected: virtual Tuple<Types...>* _alloc() volatile override { return _tuplespace.alloc(); }
	protected: virtual void _dealloc(Tuple<Types...>* tuple) volatile override { _tuplespace.dealloc(tuple); }
};
