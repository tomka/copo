#pragma once

#include <climits>
#include <float.h>

namespace cgv {
	namespace type {
		namespace traits {
			/** the max traits defines for each type \C in the static const member \c value, 
			    what the maximum value is. */
			template <typename T> struct max {};
			template <> struct max<char>  { static const char  value = CHAR_MAX; };
			template <> struct max<short> { static const short value = SHRT_MAX; };
			template <> struct max<int>   { static const int   value = INT_MAX; };
			template <> struct max<long>  { static const long  value = LONG_MAX; };
			template <> struct max<unsigned char>  { static const unsigned char  value = UCHAR_MAX; };
			template <> struct max<unsigned short> { static const unsigned short value = USHRT_MAX; };
			template <> struct max<unsigned int>   { static const unsigned int   value = UINT_MAX; };
			template <> struct max<unsigned long>  { static const unsigned long  value = ULONG_MAX; };
//			template <> struct max<float>    { static const float  value = FLT_MAX; };
//			template <> struct max<double>   { static const double value = DBL_MAX; };
		}
	}
}