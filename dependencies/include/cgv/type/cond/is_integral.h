#pragma once 

#include <cgv/type/cond/bool.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** checks if a type is an integral type */
			template <typename T>
			struct is_integral : public cond::false_type {};
			/// specializations for integral types
			template <>	struct is_integral<char> : public cond::true_type {};
			template <>	struct is_integral<short> : public cond::true_type {};
			template <>	struct is_integral<int> : public cond::true_type {};
			template <>	struct is_integral<long> : public cond::true_type {};
			template <>	struct is_integral<long long> : public cond::true_type {};
			template <>	struct is_integral<unsigned char> : public cond::true_type {};
			template <>	struct is_integral<unsigned short> : public cond::true_type {};
			template <>	struct is_integral<unsigned int> : public cond::true_type {};
			template <>	struct is_integral<unsigned long> : public cond::true_type {};
		}
	}
}
