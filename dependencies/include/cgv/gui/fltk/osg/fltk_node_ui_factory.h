//
// "$Id: cgv/gui/fltk/osg/fltk_node_ui_factor.h 4910 2006-08-02 10:00:00Z sgumhold $"
//
// templates to register factories for user interface classes of osg nodes
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
#include <cgv/registry/registry.h>
#include <cgv/gui/fltk/gui.h>
#include <cgv/gui/fltk/osg/NodeUI.h>

namespace cgv {
	namespace gui {
		namespace fltk {
			namespace osg {
		/// factory for a fltk node user interface class, don't use directly, just register with register_fltk_node_ui
		template <class FltkUI>
		struct fltk_node_ui_factory : public cgv::registry::factory
		{
			/// store the factory type name
			const char* factory_type_name;
			/// the constructor stores the factory name
			fltk_node_ui_factory(const char* _factory_type_name) : factory_type_name(_factory_type_name) {}
			/// return the factory type name 
			const char* get_name() const { return factory_type_name; }
			/// overload the construction method to work together with a constructor with the 5 arguments x,y,w,h,label
			virtual void* construct(va_list args)
			{
				int x = va_arg(args, int);
				int y = va_arg(args, int);
				int w = va_arg(args, int);
				int h = va_arg(args, int);
				const char* l = va_arg(args, const char*);
				va_end(args);
				return (cgv::gui::fltk::osg::NodeUI*) new FltkUI(x,y,w,h,l);
			}
		};

		/// use a static instance of this struct to register a fltk node ui
		template <class FltkUI>
		struct register_fltk_node_ui : public force_link
		{
			/// pass the class name of the node and the class name of the user interface class as arguments
			register_fltk_node_ui(const char* source_type_name, const char* factory_type_name)
			{
				cgv::registry::add_factory(source_type_name, "gui::fltk::osg::NodeUI", new fltk_node_ui_factory<FltkUI>(factory_type_name));
			}
		};
			}
		}
	}
}
