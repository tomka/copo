//
// "$Id: cgv/gui/fltk/config.h 4910 2006-08-02 10:00:00Z sgumhold $"
//
// methods used to start the gui
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

#pragma once
#include <cgv/config/global.h>

#include <cgv/gui/fltk/lib_begin.h>

namespace cgv {
	namespace gui {
		namespace fltk {
			/// dummy structure to ensure code linkage for constructors inside static version of lib
			struct force_link
			{
				force_link()
				{
#ifdef CGV_IS_STATIC
#pragma comment(linker, "/INCLUDE:" __FUNCDNAME__)
#endif
				}
			};

			/// run the main loop of the gui toolkit
			extern CGV_API int start_gui();

		}
	}
}
#include <cgv/config/lib_end.h>
