#pragma once 

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** the is_same template returns, whether the two template arguments are of the same type */
			template <typename T1, typename T2>
			struct is_same : public cond::false_type {};
			template <typename T>
			struct is_same<T,T> : public cond::true_type {};
		}
	}
}
