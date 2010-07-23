#pragma once

#include <cgv/type/kind.h>

namespace cgv {
	namespace type {
		namespace reflect {

/** abstract base of all types. */
struct type 
{
	/// return the kind of type
	virtual kind get_kind() const = 0;
};

		}
	}
}