#pragma once

#include <cgv/type/reflect/show.h>

namespace cgv {
	namespace type {
		namespace ds {
			/// the pair template stores two types, that are accessible via \c first and \c second
			template <typename T1, typename T2>
			struct pair
			{
				typedef T1 first;
				typedef T2 second;
			};

			template <typename T1, typename T2>
			std::ostream& operator << (std::ostream& os, const reflect::show<pair<T1,T2> >&)
			{
				return os << "(" << reflect::show<T1>() << ", " << reflect::show<T2>() << ")";
			}
		}
	}
}
