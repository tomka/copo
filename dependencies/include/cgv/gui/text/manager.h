#pragma once

#include <vector>
#include <cgv/utils/signal.h>
#include <cgv/gui/window.h>
#include <cgv/gui/view.h>
#include <cgv/gui/editor.h>
#include <cgv/gui/abst_manager.h>

namespace cgv {
	namespace gui {
		namespace text {

/** base class of all user interface implementations */
class manager : public abst_manager, public utils::signalable
{
protected:
	std::vector<menu_ptr> menu_stack;
	group_ptr app;
	bool terminated;
	int nr_keys;
	std::vector<const element*> key_bindings;
public:
	/// construct empty gui
	manager() : terminated(false) {}
	/// attach to application 
	void attach(group_ptr _app)
	{
		detach();
		app = _app;
		window* w = dynamic_cast<window*>(app.ref());
		if (w)
			attach_window(window_ptr(w));
		else
			attach_group(app);
	}
	/// detach gui manager from application group
	void detach()
	{
		if (!app.empty()) {
			window* w = dynamic_cast<window*>(app.ref());
			if (w)
				detach_window(window_ptr(w));
			else
				detach_group(app);
		}
		app.clear();
		while (!menu_stack.empty())
			menu_stack.pop_back();
	}
	/// attach to window
	void attach_window(window_ptr w)
	{
		if (w->has_menu()) {
			attach_menu(w->get_menu());
			menu_stack.push_back(w->get_menu());
		}
		attach_group(group_ptr(w.ref()));
	}
	/// detach from window
	void detach_window(window_ptr w)
	{
		if (w->has_menu())
			detach_menu(w->get_menu());
		detach_group(group_ptr(w.ref()));
	}
	/// attach to menu
	void attach_menu(menu_ptr m)
	{
		m->open.attach_method(this, &manager::toggle_menu, true, m);
		m->close.attach_method(this, &manager::toggle_menu, false, m);
		for (unsigned int i = 0; i < m->get_nr_children(); ++i) {
			element* e = m->get_child(i).ref();
			menu* s = dynamic_cast<menu*>(e);
			if (s)
				attach_menu(menu_ptr(s));
		};
	}
	/// detach from menu
	void detach_menu(menu_ptr m)
	{
		m->open.detach_method(this, &manager::toggle_menu);
		m->close.detach_method(this, &manager::toggle_menu);
		for (unsigned int i = 0; i < m->get_nr_children(); ++i) {
			element* e = m->get_child(i).ref();
			menu* s = dynamic_cast<menu*>(e);
			if (s)
				detach_menu(menu_ptr(s));
		};
	}
	/// attach to group
	void attach_group(group_ptr g)
	{
		for (unsigned int i = 0; i < g->get_nr_children(); ++i) {
			element* e = g->get_child(i).ref();
			group* cg = dynamic_cast<group*>(e);
			if (cg)
				attach_group(group_ptr(cg));
		};
	}
	/// detach from group
	void detach_group(group_ptr g)
	{
		for (unsigned int i = 0; i < g->get_nr_children(); ++i) {
			element* e = g->get_child(i).ref();
			group* cg = dynamic_cast<group*>(e);
			if (cg)
				detach_group(group_ptr(cg));
		}
	}
	/// start the main loop of the manager
	void run(int argc, char** argv)
	{
		terminated = false;
		while (!terminated && !menu_stack.empty()) {
			nr_keys = 0;
			key_bindings.clear();
			show_and_bind();
			char cmd;
			std::cin >> cmd;
			dispatch(cmd);
		};
	}
	/// terminate the gui manager
	void terminate()
	{
		terminated = true;
	}
	void toggle_menu(bool visible, menu_ptr m)
	{
		if (visible)
			menu_stack.push_back(m);
		else {
			if (menu_stack.empty())
				std::cerr << "ups in file " << __FILE__ << " at line " << __LINE__ << std::endl;
			else
				menu_stack.pop_back();
			if (menu_stack.empty())
				terminated = true;
		}
	}
	static const char* get_keys()
	{
		static const char* keys = "123456789abcdefghijklmnopqrstuvwxyz";
		return keys;
	}
	char add_key(const element* e)
	{
		++nr_keys;
		key_bindings.push_back(e);
		return get_keys()[nr_keys-1];
	}
	void show_bar(int before, const std::string& text, int length, char c) const
	{
		int i;
		for (i=0; i<before; ++i)
			std::cout << c;
		std::cout << text;
		for (i=length-(int)text.size()-before; i>0; --i)
			std::cout << c;
		std::cout << std::endl;
	}
	/// show menu
	void show_and_bind()
	{
		if (!menu_stack.empty()) {
			std::string text;
			int l = 0;
			for (unsigned int i = 0; i < menu_stack.size(); ++i) {
				text += menu_stack[i]->get_text().c_str();
				if (i+1 < menu_stack.size())
					text += " > ";
			}
			show_bar(3, text, 60, '=');
			menu_ptr m = menu_stack.back();
			for (unsigned int i=0; i<m->get_nr_children(); ++i) {
				element* e = m->get_child(i).ref();
				const label* l = dynamic_cast<const label*>(e);
				const menu* s = dynamic_cast<const menu*>(e);

				char key;
				if (l && l->is_active() || s)
					key = add_key(l);

				if (l && l->is_visible() || s) {
					if (l->is_active() || s) {
						std::cout << key << ". " << l->get_text().c_str();
						if (s)
							std::cout << " >";
						std::cout << std::endl;
					}
					else
						std::cout << "  [" << l->get_text().c_str() << "]" << std::endl;
				}
			}
			std::cout << add_key(0) << ". <" << std::endl;
		}
		show_and_bind_group(app.ref());
		std::cout << "=========>"; std::cout.flush();
	}
	void show_tab(int tab) const
	{
		for (int i=0; i<tab; ++i)
			std::cout << " ";
	}
	void show_and_bind_group(const group* g, int tab = 0)
	{
		show_bar(tab,g->get_text(), 60, '-');
		for (unsigned int i = 0; i < g->get_nr_children(); ++i) {
			element* e = g->get_child(i).ref();
			const abst_observer* ob = dynamic_cast<const abst_observer*>(e);
			if (ob) {
				show_tab(tab);
				const abst_editor* e = dynamic_cast<const abst_editor*>(ob);
				if (e)
					std::cout << add_key(ob) << ". ";
				else								
					std::cout << "   ";
				std::cout << ob->get_text() << ": ";
				abst_observed_ptr o = ob->abst_get_observed();
				if (!o.empty())
					o->abst_stream_value_out(std::cout, o->abst_get_value());
				std::cout << std::endl;
				continue;
			}
			const group* cg = dynamic_cast<const group*>(e);
			if (cg)
				show_and_bind_group(cg, tab+3);
		}
	}
	/// lookup action
	void dispatch(char cmd)
	{
		std::string::size_type pos = std::string(get_keys()).find_first_of(cmd);
		if (pos == std::string::npos)
			return;

		if (pos >= key_bindings.size())
			return;

		const element* e = key_bindings[pos];
		if (!e) {
			menu_stack.back()->close();
			return;
		}
		const button* b = dynamic_cast<const button*>(e);
		if (b) {
			b->press();
			return;
		}
		const menu* m = dynamic_cast<const menu*>(e);
		if (m) {
			m->open();
			return;
		}
		const abst_editor* ed = dynamic_cast<const abst_editor*>(e);
		if (ed) {
			std::cout << "enter new value: ";
			std::cout.flush();
			abst_observed_ptr o = ed->abst_get_observer()->abst_get_observed();
			char tmp[100];
			o->abst_stream_value_in(std::cin, tmp);
			o->abst_set_value(tmp);
			return;
		}
	}
};

		}
	}
}