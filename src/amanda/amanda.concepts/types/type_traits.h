#pragma once


#ifdef __CPP_STD__

#include <xstddef>
#include <yvals.h>

namespace type_traits
#else

#define _EMPTY_ARGUMENT		/* for empty macro argument */

#define _CLASS_DEFINE_CV(CLASS) \
	CLASS(_EMPTY_ARGUMENT) \
	CLASS(const) \
	CLASS(volatile) \
	CLASS(const volatile)

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

	template <typename T> struct remove_pointer { using type = T; };
	#define _REMOVE_POINTER(CV_OPT) template <typename T> struct remove_pointer<T * CV_OPT> { using type = T; };
	_CLASS_DEFINE_CV(_REMOVE_POINTER)
	#undef _REMOVE_POINTER
	template <typename T> using remove_pointer_t = typename remove_pointer<T>::type;

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

	template <typename T, T val> struct integral_constant
	{
		static constexpr T value = val;

		using value_type = T;
		using type = integral_constant;

		constexpr operator value_type() const noexcept { return (value); }
		constexpr value_type operator()() const noexcept { return (value); }
	};

	template <bool val> using bool_constant = integral_constant<bool, val>;
	using true_type = bool_constant<true>;
	using false_type = bool_constant<false>;

	template <typename T1, typename T2> struct is_same : false_type { };
	template <typename T> struct is_same<T, T> : true_type { };

	template <typename T, typename U> constexpr bool is_same_v = is_same<T, U>::value;
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
