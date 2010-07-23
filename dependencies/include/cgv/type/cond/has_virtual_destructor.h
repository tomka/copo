#pragma once

#include <cgv/type/cond/bool.h>

#ifndef WIN32
#include <tr1/type_traits>
#endif

namespace cgv {
	namespace type {
		namespace cond {
			/// template condition returning, whether the passed type has a virtual destructor
		#ifdef WIN32			
			template <typename T>
			struct has_virtual_destructor : 
				public cgv::type::cond::bool_type<__has_virtual_destructor(T)> // windows implementation
			{
			};
		#else
			template <typename base_type, typename derived_type>
			struct is_base_of :
				public cgv::type::cond::bool_type< std::tr1::has_virtual_destructor<T>::__value > // linux implementation
			{
			};
		#endif
		}
	}
}
