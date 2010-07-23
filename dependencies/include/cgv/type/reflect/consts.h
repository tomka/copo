#pragma once

namespace cgv {
	namespace type {
		namespace reflect {
			/// use this type to explicitly represent a bool const
			template <bool b>            struct bool_const           { static const bool           value = b; };
			/// use this type to explicitly represent a char const
			template <char c>            struct char_const           { static const char           value = c; };
			/// use this type to explicitly represent a unsigned char const
			template <unsigned char uc>  struct unsigned_char_const  { static const unsigned char  value = uc; };
			/// use this type to explicitly represent a short const
			template <short s>           struct short_const          { static const short          value = s; };
			/// use this type to explicitly represent a unsigned short const
			template <unsigned short us> struct unsigned_short_const { static const unsigned short value = us; };
			/// use this type to explicitly represent a int const
			template <int i>             struct int_const            { static const int            value = i; };
			/// use this type to explicitly represent a unsigned int  const
			template <unsigned int ui>   struct unsigned_int_const   { static const unsigned int   value = ui; };
		}
	}
}
