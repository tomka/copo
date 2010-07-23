#pragma once

#include <cgv/type/reflect/type_interface.h>

namespace cgv {
	namespace type {
		namespace reflect {
			namespace detail {
				/** abstract interface that allows to work with arbitrary types. */
				template <class T>
				struct template_type_interface : public compound_type_interface_base<template_type_interface<T> >
				{
				public:
					/// type of name range
					typedef template_interface::parameter_range parameter_range;
					/// type of name range
					typedef template_interface::parameter_range::const_iterator p_iter;
					/// return a range of parameter descriptors
					const parameter_range* pr;
					/// construct from parameter range only
					template_type_interface() : pr(T::static_get_parameter_range()) {}
					/// default construct an already allocated instance
					void default_construct(void* instance_ptr) const
					{
						char* ptr = (char*) instance_ptr;
						for (p_iter i=pr->begin(); i!= pr->end(); ++i) {
							if (i->is_typename()) {
								if (i->has_default_type())
									*((const type_interface**) ptr) = i->get_default_type();
								else
									*((const type_interface**) ptr) = 0;
								ptr += sizeof(const type_interface*);
							}
							else {
								if (i->get_type()->has_default_constructor()) {
									i->get_type()->default_construct(ptr);
								}
								if (i->has_default_value()) {
									enum dummy { A };
									switch (i->get_type()->get_kind()) {
									case BOOL : *((bool*)ptr) = i->get_default_value().v.b; break;
									case UCHAR :
									case CHAR : *((char*)ptr) = i->get_default_value().v.n.i.si.c; break;
									case UINT16:
									case INT16 : *((int16*)ptr) = i->get_default_value().v.n.i.si.s; break;
									case UINT32:
									case INT32 : *((int32*)ptr) = i->get_default_value().v.n.i.si.i; break;
									case UINT64:
									case INT64 : *((int64*)ptr) = i->get_default_value().v.n.i.si.l; break;
									case FLOAT : *((float*)ptr) = i->get_default_value().v.n.f.f; break;
									case DOUBLE : *((double*)ptr) = i->get_default_value().v.n.f.d; break;
									case WCHAR : *((wchar_t*)ptr) = i->get_default_value().v.w; break;
									}
								}
								ptr += i->get_type()->size_of();
							}
						}
					}
					/// call standard destructor only
					virtual void destruct(void* instance_ptr) const
					{
						char* ptr = (char*) instance_ptr;
						for (p_iter i=pr->begin(); i!= pr->end(); ++i) {
							if (!i->is_typename()) {
								i->get_type()->destruct(ptr);
								ptr += i->get_type()->size_of();
							}
						}
					}
					/// sizeof operator
					size_type size_of() const
					{
						size_type s = 0;
						if (s == 0) {
							for (p_iter i=pr->begin(); i!= pr->end(); ++i) {
								if (i->is_typename())
									s += sizeof(const type_interface*);
								else
									s += i->get_type()->size_of();
							}
						}
						return s;
					}
					/// return the compound descriptor if get_kind() returns STRUCT, CLASS or UNION
					const compound_descriptor*  get_compound_descriptor() const 
					{ 
						static const char* names[] = { "p0", "p1", "p2", "p3", "p4", "p5", "p6", "p7" };
						static compound_descriptor cd(this);
						if (cd.get_member_range().empty()) {
							int offset = 0;
							int j = 0;
							for (p_iter i=pr->begin(); i!= pr->end(); ++i, ++j) {
								if (i->is_typename()) {
									cd << member_descriptor(names[j],i->get_type(),this,offset,"");
									offset += sizeof(const type_interface*);
								}
								else {
									cd << member_descriptor(names[j],i->get_type(),this,offset,"");
									offset += (int)i->get_type()->size_of();
								}
							}
						}
						return &cd;
					}
				};
			}


/** \addtogroup conv_grp

	@{
*/


/** use this template as base class to your template interface implementations.
    You only need to implement the static method 

	const parameter_range* static_get_parameter_range()

	in which a static instance of the range with template parameters is constructed. */
template <typename T>
struct template_interface_base : public template_interface
{
	/// return a range of parameter descriptors
	const parameter_range* get_parameter_range() const
	{
		return T::static_get_parameter_range();
	}
	/// return a type descriptor for a type that can store the template parameters
	const type_interface* get_parameter_structure_type() const
	{
		static detail::template_type_interface<T> tti;
		return &tti;
	}
};


/** @} */

		}
	}
}