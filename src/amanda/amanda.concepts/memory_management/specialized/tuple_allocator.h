#pragma once

#include "../../types/def.h"
#include "../../types/type_traits.h"
#include "../../structures/tuple.h"
#include "../object_allocator.h"

template <typename... Types>
class TupleAllocator final
{
	private: volatile ObjectAllocator<Tuple<Types...>> _tuplespace;
	
	public: unsigned int available() volatile const { return _tuplespace.available(); }
	
	private: template <position_t i, typename Arg> static inline void _assign(Tuple<Types...>* tuple, Arg&& arg)
	{
		tuple->TupleLeaf<i, variadic_arg<i, Types...>>::value = arg;
	}
	private: template <position_t i, typename Arg, typename... Args> static inline void _assign(Tuple<Types...>* tuple, Arg&& arg, Args&&... args)
	{
		tuple->TupleLeaf<i, variadic_arg<i, Types...>>::value = arg;
		_assign<i + 1>(tuple, std::forward<Args>(args)...);
	}
	public: template <typename... Args> Tuple<Types...>* alloc(Args&&... arg) volatile
	{
		auto tuple = _tuplespace.alloc();
		_assign<0>(tuple, std::forward<Args>(arg)...);
		return tuple;
	}
	public: void dealloc(Tuple<Types...>* tuple) volatile
	{
		_tuplespace.dealloc(tuple);
	}
};
