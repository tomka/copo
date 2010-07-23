#pragma once

#include <cgv/type/cond/bool.h>
#include <cgv/type/func/clean.h>

#ifndef WIN32
#include <tr1/type_traits>
#endif

namespace cgv {
	namespace type {
		namespace cond {
			/// template condition returning, whether the type is an enum type
		#ifdef WIN32			
			template <typename T>
			struct is_enum : 
				public cgv::type::cond::bool_type<__is_enum(T)> // windows implementation
			{
			};
		#else
			template <typename T>
			struct is_enum :
				public cgv::type::cond::bool_type< std::tr1::is_enum<T>::__value > // linux implementation
			{
			};
		#endif
			/// template condition returning, whether the cleaned type of the first argument is a base class of the cleaned type of the second argument
			template <typename T>
			struct is_enum_clean : public cond::is_enum<typename func::clean<T>::type> {};
		}
	}
}
