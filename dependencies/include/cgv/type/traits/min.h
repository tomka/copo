#pragma once

#include <climits>
#include <float.h>

namespace cgv {
	namespace type {
		namespace traits {
			/** the min traits defines for each type \C in the static const member \c value, 
			    what the minimum value is. */
			template <typename T> struct min {};
			template <> struct min<char>  { static const char  value = CHAR_MIN; };
			template <> struct min<short> { static const short value = SHRT_MIN; };
			template <> struct min<int>   { static const int   value = INT_MIN; };
			template <> struct min<long>  { static const long  value = LONG_MIN; };
			template <> struct min<unsigned char>  { static const unsigned char  value = 0; };
			template <> struct min<unsigned short> { static const unsigned short value = 0; };
			template <> struct min<unsigned int>   { static const unsigned int   value = 0; };
			template <> struct min<unsigned long>  { static const unsigned long  value = 0; };
//			template <> struct min<float>    { static const float  value = -FLT_MAX; };
//			template <> struct min<double>   { static const double value = -DBL_MAX; };
		}
	}
}