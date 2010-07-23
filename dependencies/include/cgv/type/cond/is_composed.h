#pragma once 

#include <cgv/type/cond/is_enum.h>
#include <cgv/type/cond/is_fundamental.h>
#include <cgv/type/cond/is_pointer.h>
#include <cgv/type/cond/is_function_pointer.h>
#include <cgv/type/cond/is_method_pointer.h>
#include <cgv/type/cond/is_member_pointer.h>

namespace cgv {
	namespace type {
		namespace cond {
			/** checks if a type is a composed type, i.e. class, struct or union. */
			template <typename T>
			struct is_composed : public
				cond::or<cond::is_fundamental<T>, cond::is_enum<T>, 
					     cond::is_pointer<T>, cond::is_function_pointer<T>, 
						 cond::is_method_pointer<T>, cond::is_member_pointer<T>,
						 cond::is_ref<T> >
			{
			};
		}
	}
}