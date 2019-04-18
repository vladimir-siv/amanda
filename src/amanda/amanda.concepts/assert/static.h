#pragma once

template <bool condition> struct __static_assert__;
template <> struct __static_assert__<true> { static inline void assertion_check() { } };

#define xassert(expression) (__static_assert__<(expression)>::assertion_check())
