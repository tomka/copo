#pragma once

#include <cgv/type/cond/bool.h>

#ifndef WIN32
#include <tr1/type_traits>
#endif

namespace cgv {
	namespace type {
		namespace cond {
			/// template condition returning, whether the first type is convertible to the second type
		#ifdef WIN32			
			template <typename from_type, typename to_type>
			struct is_convertible_to : 
				public cgv::type::cond::bool_type<__is_convertible_to(from_type,to_type)> // windows implementation
			{
			};
		#else
			template <typename base_type, typename derived_type>
			struct is_convertible_to :
				public cgv::type::cond::bool_type< std::tr1::is_convertible_to<from_type,to_type>::__value > // linux implementation
			{
			};
		#endif
		}
	}
}
