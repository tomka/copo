#pragma once

#include <cgv/type/index_type.h>
#include <cgv/type/size_type.h>
#include <cgv/type/kind.h>
#include <string>

namespace cgv {
	namespace type {
		namespace traits {
			/// defines the kind of a given type in the static const my_kind
			template <typename T> struct kind_traits { static const kind my_kind = TYPENAME; };
			template <> struct kind_traits<bool> { static const kind my_kind = BOOL; };
			template <> struct kind_traits<char> { static const kind my_kind = CHAR; };
			template <> struct kind_traits<unsigned char> { static const kind my_kind = UCHAR; };
			template <> struct kind_traits<wchar_t> { static const kind my_kind = WCHAR; };
			template <> struct kind_traits<int16> { static const kind my_kind = INT16; };
			template <> struct kind_traits<int32> { static const kind my_kind = INT32; };
			template <> struct kind_traits<int64> { static const kind my_kind = INT64; };
			template <> struct kind_traits<uint16> { static const kind my_kind = UINT16; };
			template <> struct kind_traits<uint32> { static const kind my_kind = UINT32; };
			template <> struct kind_traits<uint64> { static const kind my_kind = UINT64; };
			template <> struct kind_traits<float> { static const kind my_kind = FLOAT; };
			template <> struct kind_traits<double> { static const kind my_kind = DOUBLE; };
			template <> struct kind_traits<std::string> { static const kind my_kind = STRING; };
			template <typename T> struct kind_traits<T*> { static const kind my_kind = POINTER; };
			template <typename T> struct kind_traits<const T*> { static const kind my_kind = POINTER; };
			template <typename T> struct kind_traits<T&> { static const kind my_kind = REFERENCE; };
			template <typename T> struct kind_traits<const T&> { static const kind my_kind = REFERENCE; };
		}
	}
}
