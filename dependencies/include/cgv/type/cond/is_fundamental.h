#pragma once 

#include <cgv/type/cond/bool.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/cond/or.h>
#include <cgv/type/cond/is_same.h>
#include <cgv/type/cond/is_integral.h>
#include <cgv/type/cond/is_numeric.h>
#include <cgv/type/cond/is_float.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** checks if a type is fundamental */
			template <typename T>
			struct is_fundamental : public or<is_numeric<T>,is_same<T,__wchar_t> > {};
		}
	}
}
