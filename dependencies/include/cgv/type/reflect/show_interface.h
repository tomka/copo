#pragma once

#include <iostream>
#include <string>
#include <cgv/type/reflect/descriptors.h>
#include <cgv/type/reflect/interfaces.h>

namespace cgv {
	namespace type {
		namespace reflect {
			/// declare wrapper type
			void show_type(std::ostream& os, const type_interface* ti, int tab=0);
			/// show the given type in ascii format on the output stream
			void show_instance(std::ostream& os, const void* instance_ptr, const type_interface* ti, int tab=0);

			namespace ShowType {
				void show(std::ostream& os, const enum_descriptor& ed, int tab=0)
				{
					os << "[";
					for (unsigned int i=0; i<ed.get_enumerator_range().size(); ++i) {
						if (i > 0)
							os << ",";
						os << ed.get_enumerator_range()[i].get_name() << "="
						   << ed.get_enumerator_range()[i].get_value();
					}
					os << "]";
				}
				void show(std::ostream& os, kind k, const pointer_descriptor& pd, int tab=0)
				{
					if (pd.is_const())
						os << "const ";
					if (k == REFERENCE) {
						os << "&";
						show_type(os, pd.get_type(), tab);
					}
					else {
						if (!pd.is_array())
							os << "*";
						show_type(os, pd.get_type(), tab);
						if (pd.is_array()) {
							os << "[" << pd.get_lwb() << "," << pd.get_hib() << "]";
						}
					}
				}
				void show_tab(std::ostream& os, int tab)
				{
					for (int j=0; j<tab; ++j)
						os << ' ';
				}
				void show(std::ostream& os, const member_descriptor& md, int tab=0)
				{
					show_type(os, md.get_member_type(), tab);
					os << " " << md.get_name();
					if (md.is_const())
						os << " const";
					os << " (" << md.get_group_index() << ")";
					if (md.get_comment())
						os << " '" << md.get_comment() << "'";
				}
				void show(std::ostream& os, kind k, const compound_descriptor& cd, int tab=0)
				{
					os << " {" << get_name_of_kind(k) << "}";
					if (cd.get_base_range().size() > 0) {
						os << " : ";
						for (unsigned int i = 0; i < cd.get_base_range().size(); ++i) {
							if (i > 0)
								os << ", ";
							os << cd.get_base_range()[i].get_type()->get_name();
						}
					}
					tab += 3;
					os << " {\n";
					for (unsigned int i = 0; i < cd.get_member_range().size(); ++i) {
						show_tab(os, tab);
						show(os,cd.get_member_range()[i],tab);
						os << "\n";
					}
					tab -= 3;
					show_tab(os, tab);
					os << "}";
				}
			}
			/// show the given type in ascii format on the output stream
			void show_type(std::ostream& os, const type_interface* ti, int tab)
			{
				if (is_fundamental(ti->get_kind()) || ti->get_kind() == STRING) {
					os << get_name_of_kind(ti->get_kind());
					if (ti->get_number_descriptor()) {
						os << "[]";
					}
				}
				else {
					switch (ti->get_kind()) {
					case POINTER :
					case REFERENCE :
						ShowType::show(os,ti->get_kind(),*ti->get_pointer_descriptor(),tab);
						break;
					case ENUM :
						os << ti->get_name();
						ShowType::show(os,*ti->get_enum_descriptor(),tab);
						break;
					case STRUCT :
					case CLASS :
					case UNION :
						os << ti->get_name();
						ShowType::show(os, ti->get_kind(), *ti->get_compound_descriptor(), tab);
						break;
					case FUNCTION_POINTER:
					case MEMBER_POINTER:
					case METHOD_POINTER:
						os << get_name_of_kind(ti->get_kind()) << " NOT IMPLEMENTED";
						break;
					}
				}				
			};
			/// show the given type in ascii format on the output stream
			void show_instance(std::ostream& os, const void* instance_ptr, const type_interface* ti, int tab)
			{
				kind k = ti->get_kind();
				if (is_fundamental(k) && !(k == ENUM)) {
					os << ((const unions::fundamental*) instance_ptr)->to_string(k).c_str();
				}
				else {
					switch (k) {
					case STRING : 
						os << ((const std::string*)instance_ptr)->c_str();
						break;
					case POINTER :
					case REFERENCE :
						{
							const pointer_descriptor* pd = ti->get_pointer_descriptor();
							os << *((const void**)instance_ptr);
							if (k == REFERENCE) {
								os << "&";
								show_instance(os, *((const void**)instance_ptr), pd->get_type(), tab);
							}
							else if (k == POINTER) {
								if (pd->is_array()) {
									os << "[\n";
									tab += 3;
									for (int i=pd->get_lwb(); i < pd->get_hib(); ++i) {
										ShowType::show_tab(os,tab);
										os << "[" << i << "]=";
										const void* element = ti->access_array_element(*((const void**)instance_ptr), i);
										show_instance(os, element, pd->get_type(), tab);
										os << "\n";
									}
									tab -= 3;
									ShowType::show_tab(os,tab);
									os << "]";
								}
								else {
									os << "*";
									show_instance(os, *((const void**)instance_ptr), pd->get_type(), tab);
								}
							}
						}
						break;
					case ENUM :
						os << *((const int*)instance_ptr);
						break;
					case STRUCT :
					case CLASS :
					case UNION :
						os << ti->get_name() << " {\n";
						tab += 3;
						{
							const compound_descriptor* cd = ti->get_compound_descriptor();
							unsigned int i;
							for (i=0; i<cd->get_base_range().size(); ++i) {
								ShowType::show_tab(os,tab);
								const base_descriptor& bd = cd->get_base_range()[i];
								show_instance(os,bd.get_base()->cast(instance_ptr),bd.get_type(),tab);
								os << "\n";
							}
							for (i=0; i<cd->get_member_range().size(); ++i) {
								ShowType::show_tab(os,tab);
								const member_descriptor& md = cd->get_member_range()[i];
								os << md.get_name() << "=";
								show_instance(os,(const char*)instance_ptr+md.get_offset(), md.get_member_type(), tab);
								os << "\n";
							}
						}
						tab -= 3;
						ShowType::show_tab(os,tab);
						os << "}";
						break;
					case FUNCTION_POINTER:
					case MEMBER_POINTER:
					case METHOD_POINTER:
						os << get_name_of_kind(k) << " NOT IMPLEMENTED";
						break;
					}
				}				
			}
		}
	}
}
