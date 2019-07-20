#pragma once

template <bool condition> struct __static_assert__;
template <> struct __static_assert__<true> { static inline void assertion_check() { } };

template <unsigned long> struct __static_value__ { };

#define xassert(expression) (__static_assert__<(expression)>::assertion_check())
#define xval(expression) (__static_value__<(expression)>::assertion_check())
