#pragma once 

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** type condition operating on type constants and checking for greater. */
			template <typename TC1, typename TC2>
			struct is_greater : public cond::bool_type<(TC1::value>TC2::value)> {};
			/** type condition operating on type constants and checking for greater or equal. */
			template <typename TC1, typename TC2>
			struct is_greater_or_equal : public cond::bool_type<(TC1::value>=TC2::value)> {};
		}
	}
}
