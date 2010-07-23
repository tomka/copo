#pragma once 

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** checks if a type has the reference modifier */
			template <typename T>
			struct is_ref: public cond::false_type {};
			template <typename T>
			struct is_ref<T&> : public cond::true_type {};
		}
	}
}
