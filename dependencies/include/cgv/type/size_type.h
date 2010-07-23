#pragma once

#ifndef _WIN32
#define __int16 short
#define __int32 int
#define __int64 long
#include <stdint.h>
#endif

#include <stddef.h>


namespace cgv {
	namespace type {
		///
		typedef unsigned __int16 uint16;
		///
		typedef unsigned __int32 uint32;
		///
		typedef unsigned __int64 uint64;
		///
		typedef uintptr_t     size_type;
	}
}
