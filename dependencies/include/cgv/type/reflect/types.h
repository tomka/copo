#pragma once

namespace cgv {
	namespace type {
		namespace reflect {
			/// type to represent void
			struct void_type {};
			/// type to represent bool
			struct bool_type {};
			/// type to represent char
			struct char_type {};
			/// type to represent unsigned char
			struct unsigned_char_type {};
			/// type to represent short
			struct short_type {};
			/// type to represent unsigned short
			struct unsigned_short_type {};
			/// type to represent int
			struct int_type {};
			/// type to represent unsigned int
			struct unsigned_int_type {};
			/// type to represent long
			struct long_type {};
			/// type to represent unsigned long
			struct unsigned_long_type {};
			/// type to represent float
			struct float_type {};
			/// type to represent double
			struct double_type {};
			/// template representing a pointer
			template <typename T>
			struct ptr_type {};
			/// template representing a reference
			template <typename T>
			struct ref_type {};
			/// template representing the const modifier
			template <typename T>
			struct const_type {};
			/// template representing a struct, where the first argument must be a type list with the base types and the second a type list with the member types
			template <typename BaseList, typename MemberList>
			struct struct_type {};
			/// template representing a class, where the first argument must be a type list with the base types and the second a type list with the member types
			template <typename BaseList, typename MemberList>
			struct class_type {};

		}
	}
}
