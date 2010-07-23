#pragma once

namespace cgv {
	namespace defines {
		struct STATIC_WARNING;

		template<bool> struct static_warning_impl;

		template<> struct static_warning_impl<false> {
			enum { value = 0 };
			typedef int type;
		};

		template<> struct static_warning_impl<true> {
			enum { value = 1 };
			struct type { 
				type() { } 
				int* operator&() { return new int; } 
			};
		};

		#define CGV_DEFINES_WARNING(B) \
			 struct CGV_DEFINES_JOIN(STATIC_WARNING, __LINE__) {   \
			   void f() {                                          \
			   ::cgv::defines::static_warning_impl<(bool)( B )>::type \
				   STATIC_WARNING;                                 \
			   }                                                   \
			 };
	}
}
