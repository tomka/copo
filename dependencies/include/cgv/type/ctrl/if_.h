#pragma once

#include <cgv/defines/warning.h>
#include <cgv/type/cond/bool.h>
#include <cgv/type/cond/is_base_of.h>

namespace cgv {
	namespace type {
		namespace ctrl {
			struct IF_CONDITION_NOT_DERIVED_FROM_TRUE_NOR_FALSE_TYPE;

			template <bool condition, typename else_type>
			struct if_impl_false
			{
				typedef else_type type; 
			};
			template <typename else_type>
			struct if_impl_false<false, else_type>
			{
				typedef int type[sizeof(IF_CONDITION_NOT_DERIVED_FROM_TRUE_NOR_FALSE_TYPE)]; 
			};
			template <bool condition, typename condition_type, typename then_type, typename else_type>
			struct if_impl
			{
				typedef then_type type; 
			};
			template <typename condition_type, typename then_type, typename else_type>
			struct if_impl<false,condition_type,then_type,else_type> : public 
				if_impl_false<cond::is_base_of<cond::false_type,condition_type>::value,else_type> {};
			/// the if traits selects a type from a given condition type which should be cond::true_type or cond::false_type
			template <typename condition_type, typename then_type, typename else_type> 
			struct if_ : public if_impl<cond::is_base_of<cond::true_type,condition_type>::value,condition_type,then_type,else_type> {};
		}
	}
}
