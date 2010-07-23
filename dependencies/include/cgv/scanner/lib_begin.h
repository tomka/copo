//
//
//
// begin block to configure defines for the scanner library
//
// Authored by Sören König
//
// This library is free software, without any warrenty
// you can redistribute it and/or modify it under the 
// terms of the GNU Library General Public License 2 or
// later as published by the Free Software Foundation
//
// Please report all bugs and problems to "cgv@inf.tu-dresden.de".
//

#ifdef CGV_SCANNER_FORCE_STATIC
#	define CGV_FORCE_STATIC_LIB
#endif
#ifdef CGV_SCANNER_DLL_EXPORTS
#	define CGV_EXPORTS
#endif

#include <cgv/config/lib_begin.h>