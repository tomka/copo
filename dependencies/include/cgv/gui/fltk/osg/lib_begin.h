// begin block to configure defines for the gui fltk osg library
//
// Authored by Stefan Gumhold
//
// Please report all bugs and problems to "cgv@inf.tu-dresden.de".

#if defined(CGV_GUI_FORCE_STATIC) || defined(CGV_GUI_FLTK_OSG_FORCE_STATIC)
#	define CGV_FORCE_STATIC_LIB
#endif
#ifdef CGV_GUI_FLTK_OSG_EXPORTS
#	define CGV_EXPORTS
#endif

#include <cgv/config/lib_begin.h>

