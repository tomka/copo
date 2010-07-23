#pragma once 

#include <cgv/type/cond/or.h>
#include <cgv/type/cond/is_integral.h>
#include <cgv/type/cond/is_float.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** checks if a type is a number */
			template <typename T>
			struct is_numeric : public or<is_integral<T>,is_float<T> > {};
		}
	}
}
