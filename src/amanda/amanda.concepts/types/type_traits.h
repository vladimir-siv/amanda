#pragma once

#ifdef __CPP_STD__
namespace type_traits
#else
namespace std
#endif
{
	template <typename... T> using void_t = void;

	template <typename T, class = void> struct _Add_reference { using _Lvalue = T; using _Rvalue = T; };
	template <typename T> struct _Add_reference<T, void_t<T&>> { using _Lvalue = T&; using _Rvalue = T&&; };
	template <typename T> struct add_lvalue_reference { using type = typename _Add_reference<T>::_Lvalue; };
	template <typename T> using add_lvalue_reference_t = typename _Add_reference<T>::_Lvalue;
	template <typename T> struct add_rvalue_reference { using type = typename _Add_reference<T>::_Rvalue; };
	template <typename T> using add_rvalue_reference_t = typename _Add_reference<T>::_Rvalue;

	template <typename T> struct remove_reference { using type = T; };
	template <typename T> struct remove_reference<T&> { using type = T; };
	template <typename T> struct remove_reference<T&&> { using type = T; };
	template <typename T> using remove_reference_t = typename remove_reference<T>::type;

	template <typename T> struct remove_const { using type = T; };
	template <typename T> struct remove_const<const T> { using type = T; };
	template <typename T> using remove_const_t = typename remove_const<T>::type;

	template <typename T> constexpr T&& forward(remove_reference_t<T>& arg) //noexcept
	{
		return static_cast<T&&>(arg);
	}
	template <typename T> constexpr T&& forward(remove_reference_t<T>&& arg) //noexcept;
	{
		return static_cast<T&&>(arg);
	}
	
	template <typename T> remove_reference_t<T>&& move(T&& arg) //noexcept
	{
		return static_cast<remove_reference_t<T>&&>(arg);
	}
}

namespace std
{
	template <typename T> struct ref_lvalue_param { using type = const T&; };
	template <typename T> struct ref_lvalue_param<T&> { using type = T&; };
	template <typename T> struct ref_lvalue_param<T&&> { using type = T&; };
	template <typename T> using ref_lvalue_param_t = typename ref_lvalue_param<T>::type;

	template <typename T> struct ref_rvalue_param { using type = T&&; };
	template <typename T> struct ref_rvalue_param<T&> { using type = T&&; };
	template <typename T> struct ref_rvalue_param<T&&> { using type = T&&; };
	template <typename T> using ref_rvalue_param_t = typename ref_rvalue_param<T>::type;
}
