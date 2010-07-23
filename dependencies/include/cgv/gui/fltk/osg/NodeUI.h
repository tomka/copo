//
// "$Id: cgv/gui/fltk/osg/NodeUI.h 4910 2006-08-02 10:00:00Z sgumhold $"
//
// abstract base class of all Node user interfaces
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
#include <fltk/TabGroup.h>
#include <osg/Node>
#include <cgv/config/global.h>

#include <cgv/gui/fltk/osg/lib_begin.h>

namespace cgv {
	namespace gui {
		namespace fltk {
			namespace osg {

	/** base class of all fltk user interfaces for the osg nodes*/
	class CGV_API NodeUI
	{
	public:
		/// empty constructor
		NodeUI() {}
		/// virtual destructor
		virtual ~NodeUI() {}
		/// integrate into tab group, don't forget to set the resizable of the tab group
		virtual void integrate(::fltk::TabGroup& tab_group) {}
		/// attach the node and increment reference count of node
		virtual void node(::osg::Node* node) {}
		/// return a pointer to the attached node
		virtual ::osg::Node* node() const { return 0; }
	};

			}
		}
	}
}

#include <cgv/config/lib_end.h>
