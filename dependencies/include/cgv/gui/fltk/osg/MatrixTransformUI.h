//
// "$Id: cgv/gui/fltk/osg/MatrixTransformUI.h 4910 2006-08-02 10:00:00Z sgumhold $"
//
// fltk ui to edit osg MatrixTransform nodes
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
#pragma warning (disable:4311)
#include <fltk/Widget.h>
#include <fltk/Group.h>
#include <fltk/FloatInput.h>
#include <osg/MatrixTransform>
#include <cgv/config/global.h>
#include <cgv/gui/fltk/osg/fltk_node_ui_factory.h>
#include <cgv/gui/fltk/osg/NodeUI.h>

#include <cgv/gui/fltk/osg/lib_begin.h>

namespace cgv {
	namespace gui {
		namespace fltk {
			namespace osg {

	/** fltk user interface for the osg MatrixTransform node*/
	class CGV_API MatrixTransformUI : public ::fltk::Group, public NodeUI
	{
	private:
		//@{
		/// call back for value changes
		void value_changed_cb(::fltk::FloatInput* floatInput);
		static inline void value_changed_cb(::fltk::Widget* w, void* v) { ((MatrixTransformUI*)v)->value_changed_cb((::fltk::FloatInput*)w); }
		//@}
	protected:
		::osg::MatrixTransform* matrix_transform; ///< pointer to the matrix transformation node
		::fltk::FloatInput* floatInputs[16];			///< array of pointers to the float inputs
		void refresh_gui();						///< call this to ensure that the float inputs contain the correct values
	public:
		/// construct 
		MatrixTransformUI(int x, int y, int w, int h, const char* label);
		/// destructor decrements reference count to node
		~MatrixTransformUI();
		/// integrate into tab group, don't forget to set the resizable of the tab group
		void integrate(::fltk::TabGroup& tab_group);
		/// attach the node and increment reference count of node
		void node(::osg::Node* node);
		/// return a pointer to the attached node
		::osg::Node* node() const;
	};
			}
		}
	}
}

#include <cgv/config/lib_end.h>
