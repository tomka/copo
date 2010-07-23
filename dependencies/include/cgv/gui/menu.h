#pragma once

#include <cgv/utils/signal.h>
#include <cgv/gui/group.h>
#include <cgv/gui/button.h>

namespace cgv {
	namespace gui {

using namespace utils;

class menu;

/// define to be used reference counted menu ptr
typedef storage::ref_ptr<menu> menu_ptr;

/** the implementation independent menu class is an item contain that
    allows selection of items */
class menu : public group
{
protected:
	/// reattach for all child buttons the button's press signals to this's select signal
	void update_signal_attachments(unsigned int from)
	{
		for (unsigned int pos = from; pos < get_nr_children(); ++pos) {
			button* b = dynamic_cast<button*>(children[pos].ref());
			if (b) {
				b->press.detach_instance(select);
				b->press.attach_instance(select, pos);
			}
		}
	}
	/// overload to ensure correct attachment of buttons to select signal
	void on_insert_child(unsigned int pos, element_ptr e) 
	{
		group::on_insert_child( pos, e);
		update_signal_attachments(pos+1);
	}
	/// overload to ensure correct attachment of buttons to select signal
	void on_remove_child(unsigned int pos)
	{
		group::on_remove_child( pos);
		update_signal_attachments(pos);
	}
	/// make visible if opened
	void on_open() 
	{
		set_visible(true);
	}
	/// hide if closed
	void on_close() 
	{
		set_visible(false);
	}
public:
	/// construct empty label, use add item to 
	menu(const std::string& _name, const std::string& _text, element_ptr _parent = element_ptr(), bool _visible = true) 
		: group(_name, _text, _parent, _visible) {
			 open.attach_method(this, &menu::on_open);
			close.attach_method(this, &menu::on_close);
	}
	/// add a new button, attach select to its press signal and return pointer to button
	button_ptr add_button(const std::string& _label)  { 
		button_ptr b = button_ptr(new button("", _label, element_ptr(this))); 
		b->press.attach_instance(select, get_nr_children());
		add_child(element_ptr(b.ref())); 
		return b; 
	}
	/// insert a new button at a given position, attach select to its press signal and return pointer to the button
	button_ptr insert_button(unsigned int pos, const std::string& _label) { 
		button_ptr b = button_ptr(new button("", _label, element_ptr(this))); 
		b->press.attach_instance(select, pos);
		insert_child(pos, element_ptr(b.ref())); 
		return b; 
	}
	/// add a new menu and return pointer to menu
	menu_ptr add_menu(const std::string& _label)
	{
		menu_ptr m = menu_ptr(new menu("", _label, element_ptr(this), false));
		add_child(element_ptr(m.ref()));
		return m;
	}
	/// add a new menu and return pointer to menu
	menu_ptr insert_menu(unsigned int pos, const std::string& _label)
	{
		menu_ptr m = menu_ptr(new menu("", _label, element_ptr(this), false));
		insert_child(pos, element_ptr(m.ref()));
		return m;
	}
	/// this signal is emitted when an element is selected
	utils::signal<unsigned int> select;
	/// call this signal to open the menu
	utils::signal<> open;
	/// call this signal to close the menu
	utils::signal<> close;
};

	}
}