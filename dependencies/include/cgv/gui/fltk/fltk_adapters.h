namespace cgv {
	namespace gui {
		namespace fltk {

void layout_group(group_ptr g, ::fltk::Group* fg, bool propagate_upward);

struct FltkWindowBase
{
	bool in_resize_from_gui;
	int last_w, last_h;
	::fltk::Rectangle old_geom;
	::fltk::Group* old_parent;
	::fltk::Window* fullscreen_window;
	const ::fltk::Window* old_window;

	FltkWindowBase()
	{
		in_resize_from_gui = false;
		last_w = last_h = -1;
		old_parent = 0;
		fullscreen_window = 0;
	}
	void on_resize(::fltk::Window* win, int w, int h)
	{
		in_resize_from_gui = true;
		win->resize(w,h);
		in_resize_from_gui = false;
	}
	void dispatch_layout(window_ptr win, ::fltk::Window* w, bool compute_layout)
	{
		if (last_w != w->w() || last_h != w->h()) {
			last_w = w->w();
			last_h = w->h();
			if (!in_resize_from_gui)
				win->resize(w->w(), w->h());
			if (compute_layout)
				layout_group(win, (::fltk::Group*)w->child(w->children()-1), false);
		}
	}
	void on_set_fullscreen(::fltk::Window* w, window::fullscreen_mode& fm)
	{
		if (fm == window::NO_FULLSCREEN) {
			if (old_parent) {
				fullscreen_window->fullscreen_off(old_geom.x(),old_geom.y(),old_geom.w(),old_geom.h());
				fullscreen_window->remove(w);
				w->parent(old_parent);
				w->show(old_window);
//				w->parent(old_parent);
				w->resize(old_geom.x(),old_geom.y(),old_geom.w(),old_geom.h());
				delete fullscreen_window;
				fullscreen_window = 0;
				old_parent = 0;
			}
		}
		else {
			if (!old_parent) {
				old_geom.x(w->x());
				old_geom.y(w->y());
				old_geom.w(w->w());
				old_geom.h(w->h());
				old_parent = w->parent();
				old_window = w->child_of();
				//::fltk::Group::current(0);
				fullscreen_window = new ::fltk::Window(::fltk::USEDEFAULT, ::fltk::USEDEFAULT, w->w(), w->h(), "fullscreen", true);
//				fullscreen_window->add(new ::fltk::Button(0,0,w->w(), w->h(),"Hello"));
//				::fltk::Group* g = new ::fltk::Group(0,0,w->w(), w->h());
//				fullscreen_window->resizable(g);
//				w->parent(0);
				fullscreen_window->add(w);
//				w->parent(fullscreen_window);
				fullscreen_window->resizable(w);
				fullscreen_window->show(0,0);
			}
			if (fm == window::FULLSCREEN_ALL) {
				fullscreen_window->fullscreen(::fltk::Monitor::all());
			}
			else {
				int mon_idx = (int&) fm;
				--mon_idx;
				const ::fltk::Monitor* p;
				int n = ::fltk::Monitor::list(&p);
				if (mon_idx >= n) {
					mon_idx = n-1;
					fm = (window::fullscreen_mode&)n;
				}
				fullscreen_window->fullscreen(p[mon_idx]);
			}
			w->resize(0,0,fullscreen_window->w(),fullscreen_window->h());
			w->show(fullscreen_window);
		}
	}
};

struct FltkWindow : public ::fltk::Window, public FltkWindowBase
{
	window_ptr win;
	FltkWindow(window_ptr _win, int x, int y, int w, int h, const char* n = 0, bool b = false) :
		::fltk::Window(x,y,w,h,n,b), win(_win)
	{
	}
	void layout()
	{
		dispatch_layout(win, this, true);
		::fltk::Window::layout();
	}
};

struct GlFltkWindow : public ::fltk::GlWindow, public FltkWindowBase
{
	gl_window_ptr win;
	GlFltkWindow(gl_window_ptr _win, int x, int y, int w, int h, const char* n = 0) :
		win(_win), ::fltk::GlWindow(x,y,w,h,n)
	{
		on_set_mode(win->get_mode());
	}
	void on_set_mode(int _m)
	{
		int fltk_mode = 0;
		if (_m & gl_window::INDEXED_COLOR)	 fltk_mode |= ::fltk::INDEXED_COLOR; 
		if (_m & gl_window::RGB_COLOR)		 fltk_mode |= ::fltk::RGB_COLOR; 
		if (_m & gl_window::SINGLE_BUFFER)	 fltk_mode |= ::fltk::SINGLE_BUFFER; 
		if (_m & gl_window::DOUBLE_BUFFER)	 fltk_mode |= ::fltk::DOUBLE_BUFFER; 
		if (_m & gl_window::ACCUM_BUFFER)	 fltk_mode |= ::fltk::ACCUM_BUFFER; 
		if (_m & gl_window::ALPHA_BUFFER)	 fltk_mode |= ::fltk::ALPHA_BUFFER; 
		if (_m & gl_window::STENCIL_BUFFER) fltk_mode |= ::fltk::STENCIL_BUFFER; 
		if (_m & gl_window::DEPTH_BUFFER)	 fltk_mode |= ::fltk::DEPTH_BUFFER; 
		mode(fltk_mode);
	}
	static int get_modifiers(int fltk_mods)
	{
		int mod = 0;
		if (fltk_mods & ::fltk::SHIFT)
			mod |= gl_window::SHIFT_MODIFIER;
		if (fltk_mods & ::fltk::CTRL)
			mod |= gl_window::CTRL_MODIFIER;
		if (fltk_mods & ::fltk::ALT)
			mod |= gl_window::ALT_MODIFIER;
		if (fltk_mods & ::fltk::META)
			mod |= gl_window::META_MODIFIER;
		if (fltk_mods & ::fltk::BUTTON1)
			mod |= gl_window::LEFT_BUTTON;
		if (fltk_mods & ::fltk::BUTTON2)
			mod |= gl_window::MIDDLE_BUTTON;
		if (fltk_mods & ::fltk::BUTTON3)
			mod |= gl_window::RIGHT_BUTTON;
		return mod;
	}
	static int get_button(int fltk_but)
	{
		switch (fltk_but) {
		case ::fltk::LeftButton : return gl_window::LEFT_BUTTON;
		case ::fltk::RightButton : return gl_window::RIGHT_BUTTON;
		case ::fltk::MiddleButton : return gl_window::MIDDLE_BUTTON;
		}
		return gl_window::NO_BUTTON;
	}
	static int get_key(int fltk_key)
	{
		switch (fltk_key) {
		case ::fltk::F1Key : return gl_window::F1_KEY;
		case ::fltk::F2Key : return gl_window::F2_KEY;
		case ::fltk::F3Key : return gl_window::F3_KEY;
		case ::fltk::F4Key : return gl_window::F4_KEY;
		case ::fltk::F5Key : return gl_window::F5_KEY;
		case ::fltk::F6Key : return gl_window::F6_KEY;
		case ::fltk::F7Key : return gl_window::F7_KEY;
		case ::fltk::F8Key : return gl_window::F8_KEY;
		case ::fltk::F9Key : return gl_window::F9_KEY;
		case ::fltk::F10Key : return gl_window::F10_KEY;
		case ::fltk::F11Key : return gl_window::F11_KEY;
		case ::fltk::F12Key : return gl_window::F12_KEY;
		case ::fltk::BackSpaceKey : return gl_window::BACKSPACE_KEY; 
		case ::fltk::TabKey	  : return gl_window::TAB_KEY;       
		case ::fltk::ReturnKey    : return gl_window::RETURN_KEY;    
		case ::fltk::PauseKey     : return gl_window::PAUSE_KEY;     
		case ::fltk::ScrollLockKey: return gl_window::SCROLLLOCK_KEY;
		case ::fltk::EscapeKey    : return gl_window::ESC_KEY;
		case ::fltk::HomeKey	  : return gl_window::HOME_KEY;      
		case ::fltk::LeftKey	  : return gl_window::LEFT_KEY;      
		case ::fltk::UpKey	  : return gl_window::UP_KEY;	          
		case ::fltk::RightKey     : return gl_window::RIGHT_KEY;     
		case ::fltk::DownKey	  : return gl_window::DOWN_KEY;      
		case ::fltk::PageUpKey    : return gl_window::PAGEUP_KEY;    
		case ::fltk::PageDownKey  : return gl_window::PAGEDOWN_KEY;  
		case ::fltk::EndKey	  : return gl_window::END_KEY;       
		case ::fltk::PrintKey     : return gl_window::PRINT_KEY;     
		case ::fltk::InsertKey    : return gl_window::INSERT_KEY;    
		case ::fltk::MenuKey	  : return gl_window::MENU_KEY;      
		case ::fltk::NumLockKey   : return gl_window::NUMLOCK_KEY;   
		case ::fltk::CapsLockKey  : return gl_window::CAPSLOCK_KEY;  
		case ::fltk::DeleteKey    : return gl_window::DELETE_KEY;
		default: return fltk_key;
		}
	}
	void on_make_current()
	{
		make_current();
	}
	void on_render()
	{
	}
	void on_redraw()
	{
		redraw();
	}
	void draw()
	{
		win->render();
	}
	void layout()
	{
		dispatch_layout(win, this, false);
		::fltk::GlWindow::layout();
	}
	int handle(int event)
	{
		switch (event) {
		case ::fltk::PUSH : 
			win->mouse_pressed(::fltk::event_x(),::fltk::event_y(),
				get_button(::fltk::event_button()),
				get_modifiers(::fltk::event_state()));
			return 1;
		case ::fltk::RELEASE :
			win->mouse_released(::fltk::event_x(),::fltk::event_y(),
				get_button(::fltk::event_button()),
				get_modifiers(::fltk::event_state()));
			return 1;
		case ::fltk::DRAG :
			win->mouse_moved(::fltk::event_x(),::fltk::event_y(),
				::fltk::event_dx(),::fltk::event_dy(),
				get_modifiers(::fltk::event_state()));
			return 1;
		case ::fltk::KEY :  // handle key events
			win->key_pressed(::fltk::event_x(),::fltk::event_y(),
				get_key(::fltk::event_key()), 
				get_modifiers(::fltk::event_state()));
			return 1;
		case ::fltk::KEYUP :  // handle key events
			win->key_released(::fltk::event_x(),::fltk::event_y(),
				get_key(::fltk::event_key()), 
				get_modifiers(::fltk::event_state()));
			return 1;
		case ::fltk::ENTER : // take keyboard focus when mouse enters
			take_focus();
			return 1;
		case ::fltk::FOCUS :   
		case ::fltk::UNFOCUS :
			return 1;
		}
		return ::fltk::GlWindow::handle(event);
	}
};

void put_parent_group(child_ptr c, ::fltk::Widget* w,
					  group_ptr& g, ::fltk::Group*& fg)
{
	element_ptr e  = c->get_parent();
	g    = group_ptr(dynamic_cast<group*>(e.ref()));
	fg = 0;
	window* win = dynamic_cast<window*>(e.ref());
	if (!g.empty()) {
		fg = w->parent();
	}
}

void put_parent_window(child_ptr c, ::fltk::Widget* w,
					   window_ptr& win, ::fltk::Window*& fwin)
{
	element_ptr e  = c->get_parent();
	win  = window_ptr(dynamic_cast<window*>(e.ref()));
	fwin = 0;
	gl_window* gl_win = dynamic_cast<gl_window*>(e.ref());
	if (!win.empty()) {
		::fltk::Group* fg = w->parent();
		if (gl_win)
			fwin = (::fltk::Window*)((FltkWindow*)fg->parent());
		else
			fwin = (::fltk::Window*)((GlFltkWindow*)fg->parent());
	}
}

		}
	}
}