#pragma once

#include <cgv/defines/join.h>
#include <cgv/type/cond/is_same.h>

namespace cgv {
	namespace defines {

		template <bool x> struct STATIC_ASSERTION_FAILURE;
		template <> struct STATIC_ASSERTION_FAILURE<true> { enum { value = 1 }; };
		template<int x> struct static_assert_test{};

#define CGV_DEFINES_ASSERT( ... ) \
	typedef ::cgv::defines::static_assert_test<\
      sizeof(::cgv::defines::STATIC_ASSERTION_FAILURE< (bool)( __VA_ARGS__ ) >)>\
         CGV_DEFINES_JOIN(_static_assert_test_, __LINE__);

#define CGV_DEFINES_ASSERT_TRUE( ... ) CGV_DEFINES_ASSERT( __VA_ARGS__ :: value)
#define CGV_DEFINES_ASSERT_FALSE( ... ) CGV_DEFINES_ASSERT(!__VA_ARGS__ :: value)
#define CGV_DEFINES_ASSERT_EQUAL( ... ) CGV_DEFINES_ASSERT(cgv::type::cond::is_same<__VA_ARGS__>::value)
#define CGV_DEFINES_ASSERT_UNEQUAL( ... ) CGV_DEFINES_ASSERT(!cgv::type::cond::is_same<__VA_ARGS__>::value)

/// this macro is used to check if a type T implements a given concept
#define CGV_DEFINES_ASSERT_CONCEPT(concept, T)\
	struct CGV_DEFINES_JOIN(_concept_assert_test_, __LINE__)\
		{\
			void (concept::concept_check<T>::*concept_ptr)();\
			CGV_DEFINES_JOIN(_concept_assert_test_, __LINE__)()\
			{\
				concept_ptr = &concept::concept_check<T>::constraints;\
			}\
		};

/// this macro is used to check if the arche type of a given concept implements the concept
#define CGV_DEFINES_ASSERT_CONCEPT_ARCHE_TYPE(concept)\
	CGV_DEFINES_ASSERT_CONCEPT(concept, concept::arche_type)

	}
}
