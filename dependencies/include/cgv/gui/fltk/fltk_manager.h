#pragma once

#include <vector>
#include <stack>
#include <map>
#include <cgv/utils/signal.h>
#include <cgv/gui/label.h>
#include <cgv/gui/menu.h>
#include <cgv/gui/window.h>
#include <cgv/gui/iterator.h>
#include <cgv/gui/view.h>
#include <cgv/gui/editor.h>
#include <cgv/gui/button.h>
#include <cgv/gui/abst_manager.h>

#pragma warning (disable:4311)
#pragma warning (disable:4275)
#pragma warning (disable:4251)
#include <fltk/run.h>
#include <fltk/draw.h>
#include <fltk/Item.h>
#include <fltk/Window.h>
#include <fltk/IntInput.h>
#include <fltk/MenuBar.h>
#include <fltk/Slider.h>
#include <fltk/ItemGroup.h>
#include <fltk/PopupMenu.h>
#include <fltk/Color.h>

#include <cgv/config/global.h>
#include <cgv/gui/fltk/lib_begin.h>


namespace cgv {
	namespace gui {
		namespace fltk {

/** manage a gui with fltk */

class CGV_API fltk_manager : 
	public abst_manager,
	public utils::signalable
{
private:
	/// used for layout
	int x,y;
protected:
	/// return iterator to all windows
	iterator all_windows() const;
	/// return iterator to shown windows
	iterator shown_windows() const;
	/// return iterator to top level windows
	iterator top_windows() const;
	/**@name styles*/
	//@{
	/// style used for menus
	::fltk::Style menu_style;
	/// style used for labels
	::fltk::Style label_style;
	/// initialize styles
	void init_styles();
	//@}
	/// keep pointer to application group
	group_ptr app;
private:
	/// keep mapping between gui elements and fltk widgets
	std::map<element*,::fltk::Widget*> element_map;
public:
	/// ask for the widget of an element
	::fltk::Widget* get_widget(element* e) const;
	/// enter a widget to the element widget map
	void register_widget(element* e, ::fltk::Widget* w);
	/// remove a widget from the element widget map
	void unregister_widget(element* e, ::fltk::Widget* w);

	/// methods that allow attachment and detachment to / from gui elements
public:
	enum tach_mode { ATTACH, DETACH };
	::fltk::Widget* tach_element (element_ptr e, tach_mode t, ::fltk::Widget* w = 0);
protected:														  				
	::fltk::Widget* tach_child   (child_ptr c,   tach_mode t, ::fltk::Widget* w = 0);
	::fltk::Widget* tach_hidable (hidable_ptr h, tach_mode t, ::fltk::Widget* w = 0);
	::fltk::Widget* tach_actable (actable_ptr a, tach_mode t, ::fltk::Widget* w = 0);
	::fltk::Widget* tach_label   (label_ptr l,   tach_mode t, ::fltk::Widget* w = 0);
	::fltk::Widget* tach_group   (group_ptr g,   tach_mode t, ::fltk::Widget* w = 0);
	::fltk::Widget* tach_window  (window_ptr wi, tach_mode t, ::fltk::Widget* w = 0);
	::fltk::Widget* tach_menu    (menu_ptr m,    tach_mode t, ::fltk::Widget* w = 0);
	::fltk::Widget* tach_button  (button_ptr b,  tach_mode t, ::fltk::Widget* w = 0);
	::fltk::Widget* tach_observer(abst_observer_ptr o, tach_mode t, ::fltk::Widget* w = 0);
	::fltk::Widget* tach_view    (abst_view* v,     tach_mode t, ::fltk::Widget* w = 0);
	::fltk::Widget* tach_editor  (abst_editor* e,   tach_mode t, ::fltk::Widget* w = 0);
public:
	/**@name implementation of abst_manager interface*/
	//@{
	/// construct empty gui
	fltk_manager(group_ptr _app = group_ptr());
	/// attach gui manager to application group
	void attach(group_ptr _app);
	/// detach gui manager from application group
	void detach();
	/// start the main loop of the manager
	void run(int argc, char** argv);
	/// terminate the gui manager
	void terminate();
	/// configure the gui manager from a text that describes properties of gui elements by name
	void configure(const std::string& text);
	//@}

	/// callbacks from gui to fltk::gui
/*	
	static void item_cb(::fltk::Widget* w, void* g);
	static void on_int_value_change(::fltk::Widget* wi, void* po);
	void on_int_update(observer<int>* o, ::fltk::FloatInput* i);

	/// attachment process
	void build_menu_item(menu* m, unsigned int& i);
	::fltk::Widget* build_sub_menu(menu* m);
	void build_menu(menu* m);
	/// attach to the menu, if any
	void attach_menu(group_ptr g);
	/// attach to a group
	void attach_group(group_ptr g);
	/// attach to a group
	void detach_group(group_ptr g);
	void toggle_menu(bool visible, menu_ptr m, ::fltk::ItemGroup* group);
	*/
};

		}
	}
}