#pragma once 

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** type condition that checks if the first type has smaller size according to the
			    \c sizeof operator. */
			template <typename TC1, typename TC2>
			struct is_smaller : public cond::bool_type<(sizeof(TC2)>sizeof(TC1))> {};
			/** type condition that checks if the first type has smaller or equal size according to the
			    \c sizeof operator. */
			template <typename TC1, typename TC2>
			struct is_smaller_or_equal : public cond::bool_type<(sizeof(TC1)<=sizeof(TC2))> {};
		}
	}
}
