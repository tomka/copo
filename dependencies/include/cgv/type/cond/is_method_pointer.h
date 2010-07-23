#pragma once

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** the is_method_pointer type is true if T is a method pointer
				pointing to a non const method and false otherwise. Only up to eight parameters
				are supported. */
			template <typename T>
			struct is_method_pointer : public cond::false_type {};

			template <typename T, typename R>
			struct is_method_pointer<R (T::*)()> : public cond::true_type {};

			template <typename T, typename R, typename P>
			struct is_method_pointer<R (T::*)(P)> : public cond::true_type {};

			template <typename T, typename R, typename P1, typename P2>
			struct is_method_pointer<R (T::*)(P1,P2)> : public cond::true_type {};

			template <typename T, typename R, typename P1, typename P2, typename P3>
			struct is_method_pointer<R (T::*)(P1,P2,P3)> : public cond::true_type {};

			template <typename T, typename R, typename P1, typename P2, typename P3, typename P4>
			struct is_method_pointer<R (T::*)(P1,P2,P3,P4)> : public cond::true_type {};

			template <typename T, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
			struct is_method_pointer<R (T::*)(P1,P2,P3,P4,P5)> : public cond::true_type {};

			template <typename T, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
			struct is_method_pointer<R (T::*)(P1,P2,P3,P4,P5,P6)> : public cond::true_type {};

			template <typename T, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
			struct is_method_pointer<R (T::*)(P1,P2,P3,P4,P5,P6,P7)> : public cond::true_type {};

			template <typename T, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
			struct is_method_pointer<R (T::*)(P1,P2,P3,P4,P5,P6,P7,P8)> : public cond::true_type {};
		}
	}
}
