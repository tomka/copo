//
// "$Id: cgv/gui/fltk/osg/Viewer.h 4910 2006-08-02 10:00:00Z sgumhold $"
//
// fltk based viewer for osg scenes
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

#pragma warning (disable:4311)
#include <fltk/Window.h>
#include <fltk/TabGroup.h>
#include <fltk/ScrollGroup.h>
#include <fltk/Browser.h>
#include <fltk/Item.h>

#include <cgv/gui/fltk/osg/NodeUI.h>
#include <cgv/gui/fltk/osg/Window.h>

#include <cgv/gui/fltk/osg/lib_begin.h>

namespace cgv {
	namespace gui {
		namespace fltk {
			namespace osg {

	/** the viewer class allows to read different osg scenes at the same time,
	    provides a tree browser through the scene graph of each loaded scene
		and a tab window that provides special user interfaces for selected
		nodes */
	class CGV_API Viewer : public ::fltk::Window
	{
	public:
		/// construct from label
		Viewer(const char* label = 0);
		/// overload show methods to avoid direct reference to fltk
		void show();
		void show(int argc, char** argv);
		/// add a scene to the viewer
		void add_scene(::osg::Node* root_node, const char* file_path = 0);
	private:
		/*@name file callbacks*/
		//@{
		void open_cb();
		static inline void open_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->open_cb(); }
		void close_cb();
		static inline void close_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->close_cb(); }
		void save_cb();
		static inline void save_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->save_cb(); }
		//@}


		/*@name view callbacks*/
		//@{
		void osg_tab_change_cb();
		static inline void osg_tab_change_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->osg_tab_change_cb(); }
		void wheel_cb();
		static inline void wheel_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->wheel_cb(); }
		void wireframe_cb();
		static inline void wireframe_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->wireframe_cb(); }
		void grid_cb();
		static inline void grid_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->grid_cb(); }
		void flat_cb();
		static inline void flat_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->flat_cb(); }
		void gouraud_cb();
		static inline void gouraud_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->gouraud_cb(); }
		void phong_cb();
		static inline void phong_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->phong_cb(); }
		//@}

		/*@name animation callbacks*/
		//@{
		void timer_cb();
		static inline void timer_cb(void* v) { ((Viewer*)v)->timer_cb(); }
		void toggle_animate_cb();
		static inline void toggle_animate_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->toggle_animate_cb(); }
		void fps_cb();
		static inline void fps_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->fps_cb(); }
		//@}

		/*@name gui update callbacks*/
		//@{
		void browser_cb(::fltk::Browser* b);
		static inline void browser_cb(::fltk::Widget* w, void* v) { ((Viewer*)v)->browser_cb((::fltk::Browser*)w); }
		//@}
	protected:
		/*@name gui construction helpers*/
		//@{
		void setStyles();
		void buildMenu();
		void build_osg_tab_group(int x, int y, int w, int h, int ww = 20, int bw = 3);
		void build_gui_tab_group(int x, int y, int w, int h, int ww = 20, int bw = 3);
		void build_gui_scroll_group(int x, int y, int w, int h, int ww = 20, int bw = 3);
		//@}

		

		/*@name gui dynamic updates*/
		//@{
		/// access to current browser
		::fltk::Browser* get_current_browser() const;
		/// construct tree view (browser) from scene graph root node
		::fltk::Browser* construct_gui_browser(::osg::Node* scene);
		/// recursive helper functions
		void add_node_to_gui_browser(::fltk::Browser* gui_browser, ::osg::Node* node, ::fltk::Group* parent);
		void add_state_set_to_gui_browser(::fltk::Browser* gui_browser, ::osg::StateSet* state_set, ::fltk::Group* parent);
		void add_drawable_to_gui_browser(::fltk::Browser* gui_browser, ::osg::Drawable* drawable, ::fltk::Group* parent);
		/// selection method if the selected scene changes
		void select_scene(Window* osg_window);
		//@}
		::fltk::Style        menu_style;			///< style used for the menu
		::fltk::Style        background_style;		///< style used for background elements
		::fltk::Style        foreground_style;		///< style used for foreground elements
		::fltk::TabGroup*    osg_tab_group;			///< pointer to the osg tab in which the osg windows are shown
		::fltk::TabGroup*    gui_tab_group;			///< pointer to the property tab in which the node guis are shown
		::fltk::Widget*      empty_osg_widget;		///< pointer to the empty widget in the osg tab
		::fltk::Widget*      empty_property_widget;	///< pointer to the empty widget in the property tab
		NodeUI*				 node_ui;				///< pointer to last used node user interface
		::fltk::ScrollGroup* gui_scroll_group;		///< scrollable group that keeps the tree browsers
		::fltk::Item*        wireframe_item;		///< store item that knows whether wireframe mode is selected (not yet used)
		::fltk::Item*        grid_item;				///< store item that knows whether grid is selected (not yet used)
		::fltk::Item*        animate_item;			///< store item that knows whether animation is turned on
		::fltk::Item*        fps_item;				///< store item that is labeled with the fps used for animation
		int                  fps;					///< store the fps used for animation
	};
			}
		}
	}
}

#include <cgv/config/lib_end.h>
