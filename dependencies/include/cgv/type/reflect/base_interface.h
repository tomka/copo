#pragma once

#include <cgv/type/reflect/interfaces.h>

namespace cgv {
	namespace type {
		namespace reflect {

/** \defgroup conv_grp Convenience Templates

	templated types that facilitate the implementation of the
	type reflection interface.

	@{
*/


/** This is a convenience base class for base interfaces that defines 
	the cast methods. 
	*/
template <class Base, class Derived, bool ta_is_virtual = false>
struct base_interface_base : public base_interface
{
	void* cast(void* parent_instance_ptr) const
	{
		return static_cast<Base*>((Derived*)parent_instance_ptr); 
	}
	const void* cast(const void* parent_instance_ptr) const
	{
		return static_cast<const Base*>((const Derived*)parent_instance_ptr); 
	}
	bool is_virtual() const { return ta_is_virtual; }
};

/** @} */

		}
	}
}