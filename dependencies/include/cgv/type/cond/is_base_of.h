#pragma once

#include <cgv/type/cond/bool.h>
#include <cgv/type/func/clean.h>

#ifndef WIN32
#include <tr1/type_traits>
#endif

namespace cgv {
	namespace type {
		namespace cond {
			/// template condition returning, whether the first argument is a base class of the second argument
		#ifdef WIN32			
			template <typename base_type, typename derived_type>
			struct is_base_of : 
				public cgv::type::cond::bool_type<__is_base_of(base_type,derived_type)> // windows implementation
			{
			};
		#else
			template <typename base_type, typename derived_type>
			struct is_base_of :
				public cgv::type::cond::bool_type< std::tr1::is_base_of<base_type,derived_type>::__value > // linux implementation
			{
			};
		#endif
			/// template condition returning, whether the cleaned type of the first argument is a base class of the cleaned type of the second argument
			template <typename base_type, typename derived_type>
			struct is_base_of_clean : public cond::is_base_of<typename func::clean<base_type>::type, typename func::clean<derived_type>::type > {};
		}
	}
}
