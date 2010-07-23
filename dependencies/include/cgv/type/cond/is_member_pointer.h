#pragma once

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** the is_member_pointer type is true if T is a pointer to a 
			    class, struct or union member. */
			template <typename T>
			struct is_member_pointer : public cond::false_type {};

			template <typename T, typename R>
			struct is_member_pointer<R (T::*)> : public cond::true_type {};
		}
	}
}
