#pragma once 

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** type condition that checks if the first type has larger size according to the
			    \c sizeof operator. */
			template <typename TC1, typename TC2>
			struct is_larger : public cond::bool_type<(sizeof(TC1)>sizeof(TC2))> {};
			/** type condition that checks if the first type has larger or equal size according to the
			    \c sizeof operator. */
			template <typename TC1, typename TC2>
			struct is_larger_or_equal : public cond::bool_type<(sizeof(TC1)>=sizeof(TC2))> {};
		}
	}
}
