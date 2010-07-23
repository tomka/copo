//
// "$Id: cgv/config/win32.h 4910 2006-08-02 10:00:00Z sgumhold $"
//
// defines that handle import export specific stuff under windows
//
// Authored by Stefan Gumhold
//
// This library is free software, without any warrenty
// you can redistribute it and/or modify it under the 
// terms of the GNU Library General Public License 2 or
// later as published by the Free Software Foundation
//
// Please report all bugs and problems to "cgv@inf.tu-dresden.de".
//

#ifdef WIN32
#   if defined(CGV_FORCE_STATIC_LIB) || defined(CGV_FORCE_STATIC)
#      define CGV_API
#      define CGV_IS_STATIC
#   else
#      ifdef CGV_EXPORTS
#         define CGV_API __declspec(dllexport)
#      else
#         ifndef __INTEL_COMPILER
#            define CGV_API __declspec(dllimport)
#         else
#            define CGV_API
#         endif
#      endif
#   endif
#else
#   define CGV_API
#endif

#undef CGV_EXPORTS
#undef CGV_FORCE_STATIC_LIB
