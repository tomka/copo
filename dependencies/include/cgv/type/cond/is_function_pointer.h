#pragma once

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** the is_function_pointer type condition is true if the given type
				is a pointer to a function and false otherwise. Only up to 8 arguments
				are supported. */
			template <typename T>
			struct is_function_pointer : public cond::false_type {};

			template <typename R>
			struct is_function_pointer<R (*)()> : public cond::true_type {};
			template <typename R, typename T1>
			struct is_function_pointer<R (*)(T1)> : public cond::true_type {};
			template <typename R, typename T1, typename T2>
			struct is_function_pointer<R (*)(T1,T2)> : public cond::true_type {};
			template <typename R, typename T1, typename T2, typename T3>
			struct is_function_pointer<R (*)(T1,T2,T3)> : public cond::true_type {};
			template <typename R, typename T1, typename T2, typename T3, typename T4>
			struct is_function_pointer<R (*)(T1,T2,T3,T4)> : public cond::true_type {};
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
			struct is_function_pointer<R (*)(T1,T2,T3,T4,T5)> : public cond::true_type {};
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
			struct is_function_pointer<R (*)(T1,T2,T3,T4,T5,T6)> : public cond::true_type {};
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
			struct is_function_pointer<R (*)(T1,T2,T3,T4,T5,T6,T7)> : public cond::true_type {};
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
			struct is_function_pointer<R (*)(T1,T2,T3,T4,T5,T6,T7,T8)> : public cond::true_type {};
		}
	}
}
