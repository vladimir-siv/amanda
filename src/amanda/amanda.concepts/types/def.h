#pragma once

#ifdef __CPP_STD__

#include <iostream>

#else

using size_t = unsigned int;

#endif

using byte = unsigned char;
using sbyte = char;
using position_t = unsigned int;

template <position_t i, typename Type, typename... Types>
struct variadic_expand
{
	using type = typename variadic_expand<i - 1, Types...>::type;
};

template <typename Type, typename... Types>
struct variadic_expand<0, Type, Types...>
{
	using type = Type;
};

template <position_t i, typename... Types>
using variadic_arg = typename variadic_expand<i, Types...>::type;

inline bool is_alphanumeric(char c) { return ('0' <= c && c <= '9' || 'A' <= c && c <= 'Z' || 'a' <= c && c <= 'z'); }
inline bool is_whitespace(char c) { return (c == ' ' || c == '\t' || c == '\r' || c == '\n'); }
