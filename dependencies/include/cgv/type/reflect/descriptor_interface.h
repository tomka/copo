#pragma once

#include <cgv/type/reflect/query_type.h>
#include <cgv/type/reflect/base_interface.h>

namespace cgv {
	namespace type {
		namespace reflect {

/** \addtogroup conv_grp

	@{
*/

/// automated construction of a member descriptor
template <typename T, class C>
member_descriptor member(const char* name, T C::*mp, const char* comment, int group_index = 0)
{
	unions::pointer_union<type_interface> mti = query_type<T>();
	const type_interface* cti = query_type<C>();
	C* c = 0;
	int offset = (int)((char*)&(c->*mp) - (char*)c);
	return member_descriptor(name,mti,cti,offset,comment,group_index);
}

/// construct a base descriptor for a non virtual base class and a derived class
template <typename Base, typename Derived>
base_descriptor base()
{
	static base_interface_base<Base,Derived,false> bi;
	const type_interface* ti = query_type<Base>();
	return base_descriptor(ti, &bi);
}
/// construct a base descriptor for a virtual base class and a derived class 
template <typename Base, typename Derived>
base_descriptor virtual_base()
{
	static base_interface_base<Base,Derived,true> bi;
	const type_interface* ti = query_type<Base>();
	return base_descriptor(ti, &bi);
}


/// construct a parameter descriptor from a given type
template <typename T>
parameter_descriptor parameter(unions::any_fundamental _default_value = unions::any_fundamental())
{
	unions::pointer_union<type_interface> tti = query_type<T>();
	return parameter_descriptor(tti,_default_value);
}

/// construct a typename parameter descriptor without default type
parameter_descriptor typename_parameter()
{
	return parameter_descriptor();
}

/// construct a typename parameter descriptor with the given type as default type
template <typename T>
parameter_descriptor typename_parameter()
{
	const type_interface* dti = query_type<T>();
	return parameter_descriptor(dti);
}

/*method_descriptor method()

constructor_descriptor constructor()
*/
/** @} */

		}
	}
}