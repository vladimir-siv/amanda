#pragma once

template<typename T> struct remove_reference { using type = T; };
template<typename T> struct remove_reference<T&> { using type = T; };
template<typename T> struct remove_reference<T&&> { using type = T; };
template<typename T> using remove_reference_t = typename remove_reference<T>::type;

namespace type_traits
{
	template<typename T> constexpr T&& forward(remove_reference_t<T>& arg) //noexcept
	{
		return static_cast<T&&>(arg);
	}
	template<typename T> constexpr T&& forward(remove_reference_t<T>&& arg) //noexcept;
	{
		return static_cast<T&&>(arg);
	}
	
	template <typename T> remove_reference_t<T>&& move(T&& arg) //noexcept
	{
		return static_cast<remove_reference_t<T>&&>(arg);
	}
}
