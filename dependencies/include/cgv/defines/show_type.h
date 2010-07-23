#pragma once

#include <cgv/defines/join.h>

namespace cgv {
	namespace defines {
		struct SHOW_TYPE {};

#define CGV_DEFINES_SHOW_TYPE( ... ) \
	static const int CGV_DEFINES_JOIN(_show_type_, __LINE__) = (int) __VA_ARGS__();
//	struct CGV_DEFINES_JOIN(_show_type_, __LINE__) {\
//		static const __VA_ARGS__ x = cgv::defines::SHOW_TYPE();\
//		cgv::defines::SHOW_TYPE f(__VA_ARGS__ v) { return (cgv::defines::SHOW_TYPE)v; }\
//	};
	}
}