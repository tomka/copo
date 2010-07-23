#pragma once

#include <cgv/gui/window.h>

namespace cgv {
	namespace gui {

/** the implementation independent window class, which implements a separate window that can be moved around, minimized and maximized. */
class gl_window : public window
{
public:
	/// different gl modes
	enum gl_mode { 
		INDEXED_COLOR = 1, 
		RGB_COLOR = 2, 
		SINGLE_BUFFER = 4, 
		DOUBLE_BUFFER = 8, 
		ACCUM_BUFFER = 16, 
		ALPHA_BUFFER = 32, 
		STENCIL_BUFFER = 64,
		DEPTH_BUFFER = 128 
	};
	/// different mouse buttons
	enum buttons {
		NO_BUTTON = 0,
		LEFT_BUTTON = 1,
		RIGHT_BUTTON = 2,
		MIDDLE_BUTTON = 4,
		WHEEL_UP = 8,
		WHEEL_DOWN = 16
	};
	/// different modifiers
	enum modifiers {
		NO_MODIFIER = 0,
		SHIFT_MODIFIER = 32,
		ALT_MODIFIER = 64,
		CTRL_MODIFIER = 128,
		META_MODIFIER = 256
	};
	/// special keys
	enum keys {
		NO_KEY = 0,
		F1_KEY = 1000,
		F2_KEY,
		F3_KEY,
		F4_KEY,
		F5_KEY,
		F6_KEY,
		F7_KEY,
		F8_KEY,
		F9_KEY,
		F10_KEY,
		F11_KEY,
		F12_KEY,
		BACKSPACE_KEY,
		TAB_KEY,
		RETURN_KEY,
		PAUSE_KEY,
		SCROLLLOCK_KEY,
		ESC_KEY,
		HOME_KEY,
		LEFT_KEY,
		UP_KEY,
		RIGHT_KEY,
		DOWN_KEY,
		PAGEUP_KEY,
		PAGEDOWN_KEY,
		END_KEY,
		PRINT_KEY,
		INSERT_KEY,
		MENU_KEY,
		NUMLOCK_KEY,
		CAPSLOCK_KEY,
		DELETE_KEY
	};
protected:
	/// store gl_mode
	int m;
	/// set different gl_mode
	virtual void on_set_mode(int _m) 
	{
		m = _m;
	}
public:
	/// construct gl_window from name, label text, gl_mode, parent, and visible flag
	gl_window(const std::string& _name, const std::string& _text, 
		      int _w = 400, int _h = 300, 
			  int _m = DOUBLE_BUFFER | RGB_COLOR | DEPTH_BUFFER,
			  element_ptr _parent = element_ptr(), bool _visible = true) 
		: window(_name, _text, _w, _h, _parent, _visible), m(_m) {
			set_mode.attach_method(this, &gl_window::on_set_mode, _0);
	}
	/// access to the current gl mode
	int get_mode() const { return m; }
	/// signal used to set a different opengl mode
	utils::signal<int> set_mode;
	/// signal used to make the opengl context current
	utils::signal<> make_current;
	/// signal emitted to render with opengl
	utils::signal<> render;
	/// signal emitted to post a redraw
	utils::signal<> redraw;
	/// signal emitted to notify key_pressed (x,y,key,state)
	utils::signal<int,int,int,int> key_pressed;
	/// signal emitted to notify key_released(x,y,key,state)
	utils::signal<int,int,int,int> key_released;
	/// signal emitted to notify mouse_move(x,y,dx,dy,state)
	utils::signal<int,int,int,int,int> mouse_moved;
	/// signal emitted to notify mouse_pressed(x,y,button,state)
	utils::signal<int,int,int,int> mouse_pressed;
	/// signal emitted to notify mouse_released(x,y,button,state)
	utils::signal<int,int,int,int> mouse_released;
};

/// define reference counted pointer to windows
typedef storage::ref_ptr<gl_window> gl_window_ptr;

	}
}