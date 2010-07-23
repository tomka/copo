#include "fltk_pch.h"

#include <cgv/gui/fltk/fltk_manager.h>
#include <cgv/gui/fltk/fltk_adapters.h>
#include <cgv/gui/fltk/fltk_manager_layout.h>

namespace cgv {
	namespace gui {
		namespace fltk {


void compute_text_size(const std::string& text, int& w, int &h)
{
	w = 0;
	h = 0;
	::fltk::Widget dummy(0,0,10,10);
//	dummy.style(label_style);
	dummy.copy_label(text.c_str());
	dummy.measure_label(w,h);
}

/********************************************************\
*                                                        *
*            tach to element                             *
*                                                        *
\********************************************************/

/// signal used to change the element name
//void on_element_set_name(::fltk::Widget* w, element_ptr e, const std::string& name)
//{
//}

::fltk::Widget* fltk_manager::tach_element (element_ptr e,       tach_mode t, ::fltk::Widget* w)
{
	child* c = dynamic_cast<child*>(e.ref());
	
	if (t == DETACH)
		w = get_widget(e.ref());

	if (c)
		w = tach_child(child_ptr(c), t, w);

	if (t == ATTACH)
		register_widget(e.ref(), w);
	else {
		unregister_widget(e.ref(), w);
		delete w;
		w = 0;
	}

	return w;
}

/// ask for the widget of an element
::fltk::Widget* fltk_manager::get_widget(element* e) const 
{
	std::map<element*,::fltk::Widget*>::const_iterator i = element_map.find(e);
	if (i != element_map.end()) 
		return i->second; 
	else
		return 0; 
}

/// enter a widget to the element widget map
void fltk_manager::register_widget(element* e, ::fltk::Widget* w)
{
	element_map[e] = w;
}

/// remove a widget from the element widget map
void fltk_manager::unregister_widget(element* e, ::fltk::Widget* w)
{
	std::map<element*,::fltk::Widget*>::iterator i = element_map.find(e);
	if (i != element_map.end())
		element_map.erase(i);
}


/********************************************************\
*                                                        *
*            tach to child                               *
*                                                        *
\********************************************************/

/// set a different parent
void on_child_set_parent(fltk_manager* m, ::fltk::Widget* w, child_ptr c, element_ptr parent)
{
	::fltk::Widget* pw = m->get_widget(parent.ref());
	::fltk::Group* pg = (::fltk::Group*) pw;
	w->parent(pg);
}

::fltk::Widget* fltk_manager::tach_child   (child_ptr c,         tach_mode t, ::fltk::Widget* w)
{
	hidable* h = dynamic_cast<hidable*>(c.ref());
	if (h)
		w = tach_hidable(hidable_ptr(h), t, w);

	if (t == ATTACH)
		c->set_parent.attach_function(on_child_set_parent, this, w, c, _0);
	else
		c->set_parent.detach_function(on_child_set_parent);

	return w;
}

/********************************************************\
*                                                        *
*            tach to hidable                             *
*                                                        *
\********************************************************/

/// signal used to change visibility state
void on_hidable_set_visible(::fltk::Widget* w, hidable_ptr h, bool visible)
{
	if (visible)
		w->show();
	else
		w->hide();
}

::fltk::Widget* fltk_manager::tach_hidable (hidable_ptr h,       tach_mode t, ::fltk::Widget* w)
{
	actable* a = dynamic_cast<actable*>(h.ref());
	if (a)
		w = tach_actable(actable_ptr(a), t, w);

	if (t == ATTACH) {
		h->set_visible.attach_function(on_hidable_set_visible, w, h, _0);
		if (!dynamic_cast<menu*>(h.ref()) && !h->is_visible())
			w->hide();
	}
	else
		h->set_visible.detach_function(on_hidable_set_visible);

	return w;
}

/********************************************************\
*                                                        *
*            tach to actable                             *
*                                                        *
\********************************************************/

/// signal used to change the active state
void on_actable_set_active(::fltk::Widget* w, actable_ptr a, bool active)
{
	if (active)
		w->activate();
	else
		w->deactivate();
}

::fltk::Widget* fltk_manager::tach_actable (actable_ptr a,       tach_mode t, ::fltk::Widget* w)
{
	label* l = dynamic_cast<label*>(a.ref());
	if (l)
		w = tach_label(label_ptr(l), t, w);

	if (t == ATTACH) {
		a->set_active.attach_function(on_actable_set_active, w, a, _0);
		if (!a->is_active())
			w->deactivate();
	}
	else
		a->set_active.detach_function(on_actable_set_active);

	return w;
}

/********************************************************\
*                                                        *
*            tach to label                               *
*                                                        *
\********************************************************/

/// signal used to change the label text
void on_label_set_text(::fltk::Widget* w, label_ptr l, const std::string& text)
{
	w->copy_label(text.c_str());
}

/// signal used to change the label text
void on_label_set_placement(::fltk::Widget* w, label_ptr l, label::placement_mode p)
{
	switch (p) {
		case label::NO_LABEL : w->align(::fltk::ALIGN_CLIP | ::fltk::ALIGN_LEFT); break;
		case label::LEFT_LABEL : w->align(::fltk::ALIGN_LEFT); break;
		case label::RIGHT_LABEL : w->align(::fltk::ALIGN_RIGHT); break;
		case label::BOTTOM_LABEL : w->align(::fltk::ALIGN_BOTTOM); break;
		case label::TOP_LABEL : w->align(::fltk::ALIGN_TOP); break;
		case label::INSIDE_LABEL : w->align(::fltk::ALIGN_INSIDE); break;
	}
	group_ptr g;
	::fltk::Group* fg = 0;
	put_parent_group(l, w, g, fg);
	if (fg)
		layout_group(g, fg, true);
}

::fltk::Widget* fltk_manager::tach_label   (label_ptr l,         tach_mode t, ::fltk::Widget* w)
{
	group*         g = dynamic_cast<group*>(l.ref());
	button*        b = dynamic_cast<button*>(l.ref());
	abst_observer* o = dynamic_cast<abst_observer*>(l.ref());

	if (g)
		w = tach_group(group_ptr(g), t, w);
	else if (b) 
		w = tach_button(button_ptr(b), t, w);
	else if (o)
		w = tach_observer(abst_observer_ptr(o), t, w);
	else if (t == ATTACH) {
		int wi, he;
		compute_text_size(l->get_text(),wi,he);
		w = new ::fltk::Widget(0,0,wi,he);
		w->style(label_style);
		w->copy_label(l->get_text().c_str());
	}

	if (t == ATTACH) {
		l->set_text.attach_function(on_label_set_text, w, l, _0);
		l->set_placement.attach_function(on_label_set_placement, w, l, _0);
	}
	else {
		l->set_text.detach_function(on_label_set_text);
		l->set_placement.detach_function(on_label_set_placement);
	}
	return w;
}

/********************************************************\
*                                                        *
*            tach to group                               *
*                                                        *
\********************************************************/

/// signal used to insert a child at a given position
void on_group_insert_child(fltk_manager* m, ::fltk::Group* fg, group_ptr g, unsigned int pos, element_ptr e)
{
	fg->begin();
		::fltk::Widget* w = m->tach_element(e, fltk_manager::ATTACH);
		fg->swap(pos, fg->children()-1);
	fg->end();
}
/// signal used to remove a child at a given position
void on_group_remove_child(fltk_manager* m, ::fltk::Group* fg, group_ptr g, unsigned int pos)
{
	m->tach_element(g->get_child(pos), fltk_manager::DETACH, fg->child(pos));
	fg->remove(pos);
}
/// signal used to swap two children
void on_group_swap_children(::fltk::Group* fg, group_ptr g, unsigned int p1, unsigned int p2)
{
	fg->swap(p1,p2);
}

/*std::ostream& operator << (std::ostream& os, ::fltk::Widget& w)
{
	return os << w.x() << "," << w.y() << ":" << w.w() << "x" << w.h();
}*/

void on_group_set_options(::fltk::Group* fg, group_ptr g)
{
	layout_group(g, fg, true);
}

::fltk::Widget* fltk_manager::tach_group   (group_ptr g,         tach_mode t, ::fltk::Widget* w)
{
	window* win = dynamic_cast<window*>(g.ref());
	menu* m = dynamic_cast<menu*>(g.ref());
	::fltk::Group* fltk_g = 0;
	::fltk::Group* fltk_g_end = 0;
	if (win) {
		w      = tach_window(window_ptr(win), t, w);
		::fltk::Window* fw = (::fltk::Window*) w;
		if (fw->children() > 0) {
			fltk_g_end = fw;
			fltk_g = (::fltk::Group*)(fw->child(fw->children()-1));
		}
	}
	else if (m) {
		w      = tach_menu(menu_ptr(m), t, w);
		fltk_g = (::fltk::Group*) w;
		fltk_g_end = fltk_g;
	}
	else {
		if (t == ATTACH)
			w = new ::fltk::Group(0,0,0,0);
		fltk_g = (::fltk::Group*) w;
		fltk_g_end = fltk_g;
	}

	if (fltk_g) {
		if (t == ATTACH) {
			// attach children
			fltk_g->begin();
			for (unsigned int i = 0; i < g->get_nr_children(); ++i)
				tach_element(g->get_child(i), t, 0);
			fltk_g_end->end();
			
			if (!m)
				layout_group(g, fltk_g, false);
			// attach signals
			g->insert_child.attach_function(on_group_insert_child, this, fltk_g, g, _0, _1);
			g->remove_child.attach_function_at(-1, on_group_remove_child, this, fltk_g, g, _0);
			g->swap_children.attach_function(on_group_swap_children, fltk_g, g, _0, _1);
			if (!m)
				g->set_options.attach_function(on_group_set_options, fltk_g, g);
		}
		else {
			// detach children
			for (unsigned int i = 0; i < g->get_nr_children(); ++i)
				tach_element(g->get_child(i), t, fltk_g->child(i));
			// detach signals
			g->insert_child.detach_function(on_group_insert_child);
			g->remove_child.detach_function(on_group_remove_child);
			g->swap_children.detach_function(on_group_swap_children);
			if (!m)
				g->set_options.detach_function(on_group_set_options);
		}
	}
	return w;
}

/********************************************************\
*                                                        *
*            tach to menu                                *
*                                                        *
\********************************************************/

void item_cb(::fltk::Widget* w, void* e) 
{ 
	button* b = (button*)e;
	b->press();
}

/// this signal is emitted when an element is selected
void on_menu_select(::fltk::Menu* g, menu_ptr m, unsigned int pos)
{
}

void on_menu_open(::fltk::Menu* fltk_parent_m, ::fltk::Menu* fltk_child_m)
{
}

void on_menu_close(fltk_manager* fm, menu_ptr m, ::fltk::Menu* fltk_ig)
{
	menu* pm = dynamic_cast<menu*>(m->get_parent().ref());
	if (!pm)
		fm->terminate();
}

::fltk::Widget* fltk_manager::tach_menu    (menu_ptr m,          tach_mode t, ::fltk::Widget* w)
{
	// check if menu is a sub menu create fltk::ItemGroup
	menu* pm = dynamic_cast<menu*>(m->get_parent().ref());
	if (pm) {
		if (t == ATTACH) {
			::fltk::ItemGroup* fltk_ig = new ::fltk::ItemGroup(m->get_text().c_str());
			w = fltk_ig;
			m->open.attach_function_at(1, on_menu_open, 
				fltk_ig, (::fltk::Menu*)get_widget(pm));
			m->close.attach_function_at(1, on_menu_close, this, m, fltk_ig);
		}
		else {
			m->open.detach_function(on_menu_open);
			m->close.detach_function(on_menu_close);
		}
	}
	// otherwise create menubar
	else {
		if (t == ATTACH) {
			::fltk::MenuBar* mb = new ::fltk::MenuBar(0,0,::fltk::Group::current()->w(),24);
			w = mb;
			w->style(&menu_style);
			m->close.attach_function_at(1, on_menu_close, this, m, mb);
		}
		else
			m->close.detach_function(on_menu_close);
	}
	// attach commonly used signal
	::fltk::Menu* fltk_m = (::fltk::Menu*) w;
	if (t == ATTACH)
		m->select.attach_function_at(1, on_menu_select, fltk_m, m, _0);
	else
		m->select.detach_function(on_menu_select);

	return w;
}

/********************************************************\
*                                                        *
*            tach to window                              *
*                                                        *
\********************************************************/


::fltk::Widget* fltk_manager::tach_window   (window_ptr win,         tach_mode t, ::fltk::Widget* w)
{
	gl_window* gl_win = dynamic_cast<gl_window*>(win.ref());
	::fltk::Window* fltk_w = 0;
	FltkWindow* fltk_my_w = 0;
	FltkWindowBase* fltk_my_base = 0;
	GlFltkWindow* fltk_my_glw = 0;

	if (t == ATTACH) {
		if (gl_win) {
			w = fltk_w = fltk_my_glw = new GlFltkWindow(gl_window_ptr(gl_win), 0,0,
											win->get_width(), win->get_height(),
											win->get_text().c_str());
			fltk_my_base = fltk_my_glw;
			/// signal used to set a different opengl mode
			gl_win->set_mode.attach_method(fltk_my_glw, &GlFltkWindow::on_set_mode, _0);
			gl_win->make_current.attach_method(fltk_my_glw, &GlFltkWindow::on_make_current);
			gl_win->render.attach_method(fltk_my_glw, &GlFltkWindow::on_render);
			gl_win->redraw.attach_method(fltk_my_glw, &GlFltkWindow::on_redraw);
		}
		else {
			int x = ::fltk::USEDEFAULT, y = ::fltk::USEDEFAULT;
			if (!win->get_parent().empty())
				x = y = 0;
			w = fltk_w = fltk_my_w = new FltkWindow(win, x, y,
											win->get_width(), win->get_height(),
											win->get_text().c_str(),x != 0);
			fltk_my_base = fltk_my_w;
		}
		win->set_fullscreen.attach_method_at(-1, fltk_my_base, &FltkWindowBase::on_set_fullscreen, fltk_w, _0);
		win->resize.attach_method(fltk_my_base, &FltkWindowBase::on_resize, fltk_w, _0, _1);
	}
	else {
		fltk_w = (::fltk::Window*) w;
		if (gl_win) {
			fltk_my_glw = (GlFltkWindow*) fltk_w;
			fltk_my_base = fltk_my_glw;
			gl_win->set_mode.detach_method(fltk_my_glw, &GlFltkWindow::on_set_mode);
			gl_win->make_current.detach_method(fltk_my_glw, &GlFltkWindow::on_make_current);
			gl_win->render.detach_method(fltk_my_glw, &GlFltkWindow::on_render);
			gl_win->redraw.detach_method(fltk_my_glw, &GlFltkWindow::on_redraw);
		}
		else
			fltk_my_base = (FltkWindow*)fltk_w;
		win->set_fullscreen.detach_method(fltk_my_base, &FltkWindowBase::on_set_fullscreen);
		win->resize.detach_method(fltk_my_base, &FltkWindowBase::on_resize);
	}
	if (!gl_win) {
		/// tach menu of window
		if (win->has_menu())
			tach_element(element_ptr(win->get_menu().ref()), t, get_widget(win->get_menu().ref()));
		if (t == ATTACH) {
			int y = win->has_menu() ? 25 : 0;
			::fltk::Group* app_group = new ::fltk::Group(0,y,fltk_w->w(),fltk_w->h()-y);
			fltk_w->end();
			fltk_w->resizable(app_group);
		}
	}
	return w;
}

/********************************************************\
*                                                        *
*            tach to button                              *
*                                                        *
\********************************************************/

void button_cb(::fltk::Widget* w, void* e) 
{ 
	button* b = (button*)e;
	b->press();
}

/// signal called when button is press
void on_button_press(::fltk::Widget* w, button_ptr b)
{
}


::fltk::Widget* fltk_manager::tach_button  (button_ptr b,        tach_mode t, ::fltk::Widget* w)
{
	if (t == ATTACH) {
		// if button is used in menu, attach/detach fltk::Item
		if (dynamic_cast<menu*>(b->get_parent().ref()))
			w = new ::fltk::Item();
		// otherwise use regular fltk::Button
		else {
			int wi = 0, he = 0;
			compute_text_size(b->get_text(),wi,he);
			w = new ::fltk::Button(0,0,wi+4,he+4);
		}
		w->copy_label(b->get_text().c_str());
		w->callback(item_cb, b.ref());
		b->press.attach_function(on_button_press, w, b);
	}
	else {
		w->callback((::fltk::Callback*)0);
		b->press.detach_function(on_button_press);
	}
	return w;
}

/********************************************************\
*                                                        *
*            tach to observer                            *
*                                                        *
\********************************************************/

void on_int_value_change(::fltk::Widget* wi, void* po)
{
	observer<int>* o = (observer<int>*) po;
	::fltk::FloatInput* i = (::fltk::FloatInput*) wi;
	o->get_observed()->set_value(i->ivalue());
}
void on_int_update(observer<int>* o, ::fltk::FloatInput* i)
{
	i->value(o->get_observed()->get_value());
}

::fltk::Widget* fltk_manager::tach_observer(abst_observer_ptr o, tach_mode t, ::fltk::Widget* w)
{
	abst_view*   v = dynamic_cast<abst_view*>(o.ref());
	abst_editor* e = dynamic_cast<abst_editor*>(o.ref());
	if (v)
		w = tach_view(v, t, w);
	else if (e)
		w = tach_editor(e, t, w);
	return w;
}


/********************************************************\
*                                                        *
*            tach to view                                *
*                                                        *
\********************************************************/

void on_int_observer_update(int_observer_ptr io, ::fltk::FloatInput* fi)
{
	fi->value(io->get_observed()->get_value());
}

::fltk::Widget* fltk_manager::tach_view    (abst_view* v,     tach_mode t, ::fltk::Widget* w)
{
	view<int>*    int_v    = dynamic_cast<view<int>*>(v);
	if (int_v) {
		if (t == ATTACH) {
			int wi, he;
			compute_text_size("00000000",wi,he);
			::fltk::FloatInput* i = new ::fltk::FloatInput(0,0,wi+8,he+6);
			w = i;
			i->deactivate();
			i->type(::fltk::FloatInput::INT);
			i->value(int_v->get_observed()->get_value());
			i->copy_label(v->get_observer()->get_text().c_str());
			i->flags(::fltk::ALIGN_LEFT);
			int_v->update.attach_function(&on_int_observer_update, int_observer_ptr(int_v), i);
		}
		else {
			int_v->update.detach_function(&on_int_observer_update);
		}
	}
	return w;
}

/********************************************************\
*                                                        *
*            tach to editor                              *
*                                                        *
\********************************************************/

void int_editor_cb(::fltk::Widget* w, void* p)
{
	::fltk::FloatInput* i = (::fltk::FloatInput*) w;
	editor<int>* int_e = (editor<int>*) p;
	int_e->get_observed()->set_value(i->ivalue());
}

::fltk::Widget* fltk_manager::tach_editor  (abst_editor* e,   tach_mode t, ::fltk::Widget* w)
{
	editor<int>*    int_e    = dynamic_cast<editor<int>*>(e);
	if (int_e) {
		if (t == ATTACH) {
			int wi = 0, he = 0;
			compute_text_size("00000000",wi,he);
			::fltk::FloatInput* i = new ::fltk::FloatInput(0,0,wi+8,he+8);
			w = i;
			i->type(::fltk::FloatInput::INT);
			i->value(int_e->get_observed()->get_value());
			int_e->update.attach_function(&on_int_observer_update, int_observer_ptr(int_e), i);
			i->callback(int_editor_cb, int_e);
		}
		else {
			int_e->update.detach_function(&on_int_observer_update);
		}
	}
	return w;
}

/// construct empty gui
fltk_manager::fltk_manager(group_ptr _app)
{
	if (!_app.empty())
		attach(_app);
}

void fltk_manager::init_styles()
{
	iterator i = all_windows();
	if (i.at_end())
		return;
	::fltk::Window* main_window = (::fltk::Window*)get_widget(*i);
	// different styles
	menu_style.parent_ = main_window->style();
	menu_style.color(::fltk::color(200,200,255));
	menu_style.textcolor(::fltk::color(0,0,100));
	menu_style.box(::fltk::UP_BOX);
	menu_style.buttonbox(::fltk::HIGHLIGHT_DOWN_BOX);

	label_style.parent_ = main_window->style();
}
/*
void fltk_manager::build_menu_item(menu* m, unsigned int& i)
{
	::fltk::Widget* w = 0;

	element* e = m->get_child(i);
	if (i+1 < (int)m->get_nr_children() &&
		dynamic_cast<button*>(e) &&
		dynamic_cast<menu*>(m->get_child(i+1).ref()) ) {
			// sub menu case
			++i;
			e = m->get_child(i);
			w = build_sub_menu(dynamic_cast<menu*>(e));
	}
	else {
		button* b = dynamic_cast<button*>(e);
		if (b) {
			::fltk::Item* item = new ::fltk::Item();
			item->copy_label(b->get_text().c_str());
			item->callback(item_cb, b);
			w = item;
		}
	}
	if (w) {
		group* g = dynamic_cast<group*>(e);
		if (g) {
			g->insert_child.attach_function_at(1, on_group_insert_child, g, _0, w);
			g->remove_child.attach_function_at(-1, on_group_remove_child, g, _0, w);
		}
		cgv::gui::label* l = dynamic_cast<cgv::gui::label*>(e);
		if (l) {
			l->set_text.attach_function_at(1, on_label_set_text, w, _0);
		}
		hidable* h = dynamic_cast<hidable*>(e);
		if (h) {
			if (!g && !h->is_visible())
				w->hide();
			h->set_visible.attach_function_at(1, on_hidable_set_visible, w, _0);
		}
		actable* a = dynamic_cast<actable*>(e);
		if (a) {
			if (!g && !a->is_active())
				w->deactivate();
			a->set_active.attach_function_at(1, on_actable_set_active, w, _0);
		}
	}
}
::fltk::Widget* fltk_manager::build_sub_menu(menu* m)
{
	::fltk::ItemGroup* group = new ::fltk::ItemGroup(m->get_text().c_str());
	m->set_visible.attach_method_at(1, this, &fltk_manager::toggle_menu, _0, menu_ptr(m), group);
	group->begin();
		for (unsigned int i = 0; i < m->get_nr_children(); ++i)
			build_menu_item(m, i);
	group->end();
	return group;
}
void fltk_manager::build_menu(menu* m)
{
	menubar = new ::fltk::MenuBar(0,0,main_window->w(),24);
	m->set_visible.attach_method_at(1, this, &fltk_manager::terminate);
	menubar->style(&menu_style);
	menubar->begin();
		for (unsigned int i = 0; i < m->get_nr_children(); ++i)
			build_menu_item(m, i);
	menubar->end();
}
/// t == ATTACH to the menu, if any
void fltk_manager::attach_menu(group_ptr g)
{
	for (unsigned int i = 0; i < g->get_nr_children(); ++i) {
		menu* m = dynamic_cast<menu*>(g->get_child(i).ref());
		if (m) {
			if (!menubar)
				build_menu(m);
			//attach_menu(group_ptr(m));
			continue;
		}
	}
}
*/

/*
void fltk_manager::on_int_value_change(::fltk::Widget* wi, void* po)
{
	observer<int>* o = (observer<int>*) po;
	::fltk::FloatInput* i = (::fltk::FloatInput*) wi;
	o->get_observed()->set_value(i->ivalue());
}
void fltk_manager::on_int_update(observer<int>* o, ::fltk::FloatInput* i)
{
	i->value(o->get_observed()->get_value());
}
*/

/*
/// t == ATTACH to a group
void fltk_manager::attach_group(group_ptr g)
{
	int width = 0, height = 0;
	// determine label width
	unsigned int i;
	for (i = 0; i < g->get_nr_children(); ++i) {
		element* e = g->get_child(i);
		cgv::gui::label* l = dynamic_cast<cgv::gui::label*>(e);
		if (l) {
			int wi = 0, he = 0;
			::fltk::Widget* w = new ::fltk::Widget(x,y,10,10);
			w->style(label_style);
			w->copy_label(l->get_text().c_str());
			w->textsize(20);
			w->measure_label(wi,he);
			delete w;
			if (wi > width) width = wi;
			if (he > height) height = he;
		}
	}

	for (i = 0; i < g->get_nr_children(); ++i) {
		element* e = g->get_child(i);
		if (dynamic_cast<menu*>(e))
			continue;
		cgv::gui::label* l = dynamic_cast<cgv::gui::label*>(e);
		int he = 0;
		if (l) {
			int wi = 0;
			::fltk::Widget* w = new ::fltk::Widget(x,y,10,10);
			w->style(label_style);
			w->copy_label(l->get_text().c_str());
//				w->textsize(20);
			w->measure_label(wi,he);
			w->resize(x,y,width+5,he+2);
		}
		observer<int>* o = dynamic_cast<observer<int>*>(e);
		if (o) {
			::fltk::FloatInput* i = new ::fltk::FloatInput(x+width+10,y,50,15);
			i->type(::fltk::FloatInput::INT);
			i->value(o->get_observed()->get_value());
//				i->textsize(20);
			o->update.attach_method(this, &fltk_manager::on_int_update, o, i);
			if (dynamic_cast<view<int>*>(e))
				i->deactivate();
			else {
				i->callback(&fltk_manager::on_int_value_change, o);
			}
		}
		y += he+5;
	}
}

/// t == ATTACH to a group
void fltk_manager::detach_group(group_ptr g)
{
	for (unsigned int i = 0; i < g->get_nr_children(); ++i) {
		menu* m = dynamic_cast<menu*>(g->get_child(i).ref());
		if (m) {
			m->set_visible.detach_instance(this);
			detach_group(group_ptr(m));
			continue;
		}
	}
}

*/

/// attach gui fltk_manager to application group
void fltk_manager::attach(group_ptr _app)
{
	detach();
	app = _app;
	if (app.empty())
		return;

	// attach all windows
	for (iterator i=top_windows(); !i.at_end(); i.goto_next()) {
		window* win = dynamic_cast<window*>((*i).ref());
		tach_element(element_ptr(win), ATTACH, get_widget(win));
	}
	init_styles();

	/*
	main_window = new ::fltk::Window(::fltk::USEDEFAULT,::fltk::USEDEFAULT,480,320,"gui",false);
	main_window->copy_label(app->get_text().c_str());
	init_styles();
	main_window->begin();
		attach_menu(_app);
		y = menubar ? 25 : 0;
		app_group = new ::fltk::Group(0,y,main_window->w(),main_window->h()-y);
		app_group->begin();
			x = 5;
			y = 5;
			attach_group(_app);
		app_group->end();
	main_window->end();*/
}
/// detach gui fltk_manager from application group
void fltk_manager::detach()
{
	if (app.empty())
		return;

	// detach all windows
	for (iterator i=top_windows(); !i.at_end(); i.goto_next()) {
		window* win = dynamic_cast<window*>((*i).ref());
		tach_element(element_ptr(win), DETACH, get_widget(win));
	}

		/*
		detach_group(app);
	if (menubar) {
		delete menubar;
		menubar = 0;
	}
	if (app_group) {
		delete app_group;
		app_group = 0;
	}
	if (main_window) {
		delete main_window;
		main_window = 0;
	}*/
}
void fltk_manager::run(int argc, char** argv)
{
	// show all windows
	for (iterator i=top_windows(); !i.at_end(); i.goto_next()) {
		((::fltk::Window*)get_widget(*i))->show(argc,argv);
	}
	::fltk::run();
}
void fltk_manager::terminate()
{
	// hide all windows
	for (iterator i=top_windows(); !i.at_end(); i.goto_next())
		((::fltk::Window*)get_widget(*i))->hide();
}

/// configure the gui manager from a text that describes properties of gui elements by name
void fltk_manager::configure(const std::string& text)
{
}

bool is_shown_window(element_ptr e)
{
	window* win = dynamic_cast<window*>(e.ref());
	return win && win->is_visible();
}

bool is_window(element_ptr e)
{
	window* win = dynamic_cast<window*>(e.ref());
	return win && win->is_visible();
}

bool always_descent(group_ptr)
{
	return true;
}

bool descent_non_windows(group_ptr g)
{
	if (dynamic_cast<window*>(g.ref()))
		return false;
	return true;
}

/// return iterator to shown windows
iterator fltk_manager::shown_windows() const
{
	return iterator(app, is_shown_window, always_descent);
}

/// return iterator to shown windows
iterator fltk_manager::all_windows() const
{
	return iterator(app, is_window, always_descent);
}

/// return iterator to top level windows
iterator fltk_manager::top_windows() const
{
	return iterator(app, is_window, descent_non_windows);
}

/*
void fltk_manager::toggle_menu(bool visible, menu_ptr m, ::fltk::ItemGroup* group)
{
	std::cout << "toggle_menu" << std::endl;
}


void fltk_manager::on_insert_child(group* g, unsigned int pos, ::fltk::Widget* w)
{
	menu* m = dynamic_cast<menu*>(g);
	if (m) {
		::fltk::ItemGroup* ig = (::fltk::ItemGroup*) w;
		ig->begin();
			build_menu_item(m, pos);
		ig->end();
		ig->swap(pos, g->get_nr_children()-1);
	}
}
void fltk_manager::on_remove_child(group* g, unsigned int pos, ::fltk::Widget* w)
{
	menu* m = dynamic_cast<menu*>(g);
	if (m) {
		::fltk::ItemGroup* ig = (::fltk::ItemGroup*) w;
		::fltk::Widget* c = ig->child(pos);
		ig->remove(pos);
		delete c;
	}
}
*/
		}
	}
}