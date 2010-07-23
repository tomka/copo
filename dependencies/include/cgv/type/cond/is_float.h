#pragma once 

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** checks if a type is a floating point number */
			template <typename T>
			struct is_float : public cond::false_type {};
			/// specializations for floating point types
			template <>	struct is_float<float> : public cond::true_type {};
			template <>	struct is_float<double> : public cond::true_type {};
			template <>	struct is_float<long double> : public cond::true_type {};
		}
	}
}
