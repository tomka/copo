#pragma once

#include <iostream>
#include <cgv/type/cond/is_base_of.h>
#include <cgv/type/reflect/types.h>

namespace cgv {
	namespace type {
		namespace reflect {
			/// declare wrapper type
			template <typename T> struct show {};
			/// standard implementation prints >unknown type<
			template <typename T>		
			inline std::ostream& operator << (std::ostream& os, const show<T>&)
			{
				if (cond::is_base_of<cond::false_type,T>::value)
					return os << "false_type";
				if (cond::is_base_of<cond::true_type,T>::value)
					return os << "true_type";
				if (cond::is_const<T>::value)
					return os << "const " << show<func::drop_const<T>::type>();
				if (cond::is_ref<T>::value)
					return os << show<func::drop_ref<T>::type>() << "&";
				return os << ">unknown type<";
			};
			/// specialization for pointer types
			template <typename T>		
			inline std::ostream& operator << (std::ostream& os, const show<T*>&)
			{
				return os << show<T>() << "*";
			};
			inline std::ostream& operator << (std::ostream& os, const show<char_type>&)           { return os << "char_type"; }
			inline std::ostream& operator << (std::ostream& os, const show<unsigned_char_type>&)  { return os << "unsigned_char_type"; }
			inline std::ostream& operator << (std::ostream& os, const show<short_type>&)          { return os << "short_type"; }
			inline std::ostream& operator << (std::ostream& os, const show<unsigned_short_type>&) { return os << "unsigned_short_type"; }
			inline std::ostream& operator << (std::ostream& os, const show<int_type>&)            { return os << "int_type"; }
			inline std::ostream& operator << (std::ostream& os, const show<unsigned_int_type>&)   { return os << "unsigned_int_type"; }
			inline std::ostream& operator << (std::ostream& os, const show<float_type>&)          { return os << "float_type"; }
			inline std::ostream& operator << (std::ostream& os, const show<double_type>&)         { return os << "double_type"; }

			inline std::ostream& operator << (std::ostream& os, const show<bool>&)           { return os << "bool"; }
			inline std::ostream& operator << (std::ostream& os, const show<char>&)           { return os << "char"; }
			inline std::ostream& operator << (std::ostream& os, const show<unsigned char>&)  { return os << "unsigned char"; }
			inline std::ostream& operator << (std::ostream& os, const show<short>&)          { return os << "short"; }
			inline std::ostream& operator << (std::ostream& os, const show<unsigned short>&) { return os << "unsigned short"; }
			inline std::ostream& operator << (std::ostream& os, const show<int>&)            { return os << "int"; }
			inline std::ostream& operator << (std::ostream& os, const show<unsigned int>&)   { return os << "unsigned int"; }
			inline std::ostream& operator << (std::ostream& os, const show<float>&)          { return os << "float"; }
			inline std::ostream& operator << (std::ostream& os, const show<double>&)         { return os << "double"; }
		}
	}
}
