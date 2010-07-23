#pragma once 

#include <cgv/type/cond/bool.h>
#include <cgv/type/ctrl/if_.h>

namespace cgv {
	namespace type {
		namespace cond {
			/// implement boolean not operator
			template <typename T>
			struct not : ctrl::if_<T,cond::false_type,cond::true_type>::type {};
		}
	}
}
