#pragma once

#include "tuple_allocator.h"

#include "../../assert/static.h"

template <typename T, typename... Types>
class IReinterpretingAllocator
{
	private: void __constraints__() volatile const { xassert(sizeof(T) == sizeof(Tuple<Types...>)); }
	
	public: IReinterpretingAllocator() { __constraints__(); }
	public: virtual ~IReinterpretingAllocator() { }
	
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
	
	// Warning: object may be ill-formed (not properly initialized)
	public: T* alloc() volatile
	{
		auto tuple = _alloc();
		return reinterpret_cast<T*>(tuple);
	}
	public: template <typename... Args> T* alloc(Args&&... arg) volatile
	{
		auto tuple = _alloc();
		_assign<0>(tuple, std::forward<Args>(arg)...);
		return reinterpret_cast<T*>(tuple);
	}
	public: void dealloc(T* object) volatile
	{
		_dealloc(reinterpret_cast<Tuple<Types...>*>(object));
	}
};

template <size_t size, typename T, typename... Types>
class ReinterpretingAllocator : public IReinterpretingAllocator<T, Types...>
{
	protected: volatile TupleAllocator<size, Types...> _allocator;
	
	public: virtual unsigned int available() volatile const override { return _allocator.available(); }
	
	protected: virtual Tuple<Types...>* _alloc() volatile override { return _allocator.alloc(); }
	protected: virtual void _dealloc(Tuple<Types...>* tuple) volatile override { _allocator.dealloc(tuple); }
};
