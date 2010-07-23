#pragma once

namespace cgv {
	namespace type {
		namespace traits {
			/** the one traits defines for each type \C in the static const member \c value, 
			    what the value of one is. */
			template <typename T> struct one { static const T value = 1; };
		}
	}
}