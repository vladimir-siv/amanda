#pragma once

#include "../types/def.h"
#include "../types/type_traits.h"

template <position_t i, typename Type>
struct TupleLeaf
{
	public: Type value;
	public: TupleLeaf() { }
	public: template <typename T> TupleLeaf(T&& value) : value(std::forward<T>(value)) { }
};

template <position_t i, typename... Types>
struct TupleImpl;

template <position_t i>
struct TupleImpl<i> { };

template <position_t i, typename Type, typename... Types>
struct TupleImpl<i, Type, Types...> :
	public TupleLeaf<i, Type>,
	public TupleImpl<i + 1, Types...>
{
	public: TupleImpl() { }
	public: template <typename T, typename... Parameters>
	TupleImpl(T&& value, Parameters&&... rest) :
		TupleLeaf<i, Type>(std::forward<T>(value)),
		TupleImpl<i + 1, Types...>(std::forward<Parameters>(rest)...)
	{ }
};

template <typename... Types>
struct Tuple final : public TupleImpl<0, Types...>
{
	public: Tuple() { }
	public: template <typename... Parameters>
	Tuple(Parameters&&... value) : TupleImpl<0, Types...>(std::forward<Parameters>(value)...) { }
	
	public: template <typename position_t i>
	variadic_arg<i, Types...>& e()
	{
		return this->TupleLeaf<i, variadic_arg<i, Types...>>::value;
	}
};
