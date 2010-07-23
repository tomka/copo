#pragma once

#include <cgv/type/if_traits.h>

/// start templated definition with concept based implementation selection of struct A
#define CGV_DEFINES_TEMPLATED_STRUCT(A) \
struct A##_standard

/// define first concept based implementation of struct A with concept in namespace NS and name C
#define CGV_DEFINES_TEMPLATED_STRUCT_OF_CONCEPT_FIRST(A,NS,C) \
struct A##_traits_##C; \
template <typename T> \
struct A##_##C; \
template <typename T> \
struct A##_traits \
{ \
	typedef typename cgv::type::if_traits<typename NS::C::traits<T>::implements, \
	A##_##C<T>,typename A##_traits_##C<T>::type>::type type; \
}; \
template <typename T> \
struct A##_##C

/// define next concept based implementation of struct A with concept in namespace NS and name C, where previous concept is named PC
#define CGV_DEFINES_TEMPLATED_STRUCT_OF_CONCEPT_NEXT(A,NS,C,PC) \
struct A##_traits_##C; \
template <typename T> \
struct A##_##C; \
template <typename T> \
struct A##_traits_##PC \
{ \
	typedef typename cgv::type::if_traits<typename NS::C::traits<T>::implements, \
	A##_##C<T>,typename A##_traits_##C<T>::type>::type type; \
}; \
template <typename T> \
struct A##_##C

/// finish implementation of A, where last concept was named PC
#define CGV_DEFINES_TEMPLATED_STRUCT_END(A, PC) \
template <typename T> \
struct A##_traits_##PC \
{ \
	typedef A##_standard<T> type; \
}; \
template <typename T> \
struct A : public A##_traits<T>::type {};
