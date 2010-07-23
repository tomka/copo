#pragma once 

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** type condition operating on type constants and checking for less. */
			template <typename TC1, typename TC2>
			struct is_less : public cond::bool_type<(TC2::value>TC1::value)> {};
			/** type condition operating on type constants and checking for less or equal. */
			template <typename TC1, typename TC2>
			struct is_less_or_equal : public cond::bool_type<(TC1::value<=TC2::value)> {};
		}
	}
}
