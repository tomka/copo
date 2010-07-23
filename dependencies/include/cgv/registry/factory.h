//
// "$Id: cgv/registry/factor.h 4910 2006-08-02 10:00:00Z sgumhold $"
//
// a factory allows to construct an instance of an object
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
#include <stdarg.h>
#include <cgv/config/global.h>

namespace cgv {
	namespace registry {
		/// abstract base class of all factories
		struct factory
		{
			/// virtual constructor
			virtual ~factory() {}
			/// overload to name the factory
			virtual const char* get_name() const { return "factory"; }
			/// overload to construct an instance, pass args to class constructor
			virtual void* construct(va_list args) { return 0; }
		};
	}
}
