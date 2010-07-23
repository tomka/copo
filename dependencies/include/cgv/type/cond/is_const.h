#pragma once 

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** checks if a type has the const modifier */
			template <typename T>
			struct is_const : public cond::false_type {};
			template <typename T>
			struct is_const<const T> : public cond::true_type {};
			template <typename T>
			struct is_const<T&> : public is_const<T> {};
		}
	}
}
