#pragma once 

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** checks if a type is a pointer type */
			template <typename T>
			struct is_pointer: public cond::false_type {};
			template <typename T>
			struct is_pointer<T*> : public cond::true_type {};
		}
	}
}
