#pragma once

#include <cgv/gui/menu.h>

namespace cgv {
	namespace gui {

/** the implementation independent window class, which implements a separate window that can be moved around, minimized and maximized. */
class window : public group
{
public:
	/// different fullscreen modes
	enum fullscreen_mode { 
		NO_FULLSCREEN = 0, FULLSCREEN_1 = 1, FULLSCREEN_2 = 2, FULLSCREEN_3 = 3, 
						   FULLSCREEN_4 = 4, FULLSCREEN_5 = 5, FULLSCREEN_6 = 6, 
						   FULLSCREEN_7 = 7, FULLSCREEN_8 = 8, FULLSCREEN_ALL = 255 };
protected:
	/// store a pointer to the menu
	menu_ptr m;
	/// insert a child at a given position
	virtual void on_set_menu(menu_ptr _m) { m = _m; }
	/// store width and height
	int w, h;
	/// store new size of window
	virtual void on_resize(int _w, int _h)
	{
		w = _w;
		h = _h;
	}
	/// fullscreen mode
	fullscreen_mode fm;
	/// insert a child at a given position
	virtual void on_set_fullscreen(fullscreen_mode _fm) { fm = _fm; }
public:
	/// construct window from name, label text, parent, and visible flag
	window(const std::string& _name, const std::string& _text, int _w = 400, int _h = 300, element_ptr _parent = element_ptr(), bool _visible = true) 
		: group(_name, _text, _parent, _visible) {
			set_menu.attach_method(this, &window::on_set_menu, _0);
			set_fullscreen.attach_method(this, &window::on_set_fullscreen, _0);
			w = _w;
			h = _h;
			fm = NO_FULLSCREEN;
	}
	/// return whether a menu is attached to window
	bool has_menu() const { return !m.empty(); }
	/// access to menu
	menu_ptr get_menu() const { return m; }
	/// access to width
	int get_width() const { return w; }
	/// access to height
	int get_height() const { return h; }
	/// access to fullscreen mode
	fullscreen_mode get_fullscreen() const { return fm; }
	/// signal used to set the menu
	utils::signal<menu_ptr> set_menu;
	/// set fullscreen mode with a reference to the mode, which allows to correct a not supported mode
	utils::signal<fullscreen_mode> set_fullscreen;
	/// set width and height of window
	utils::signal<int,int> resize;
};

/// define reference counted pointer to windows
typedef storage::ref_ptr<window> window_ptr;

	}
}