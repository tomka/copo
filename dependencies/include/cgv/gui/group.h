#pragma once

#include <vector>
#include <algorithm>
#include <cgv/gui/button.h>

namespace cgv {
	namespace gui {

/** the implementation independent group class is a container for gui children. */
class group : public label
{
public:
	/// different group options
	enum options { VERTICAL = 1, HORIZONTAL = 2, HSCROLL = 4, VSCROLL = 8, TAB = 16 };
protected:
	/// store vector of children
	std::vector<element_ptr> children;
	/// store group options
	options o;
	/// insert a child at a given position
	virtual void on_insert_child(unsigned int pos, element_ptr e) 
	{
		if (pos == children.size())
			children.push_back(e);
		else
			children.insert(children.begin()+pos, e);
		child* c = dynamic_cast<child*>(e.ref());
		if (c)
			c->set_parent(element_ptr(this));
	}
	/// remove a given child
	virtual void on_remove_child(unsigned int pos) 
	{
		child* c = dynamic_cast<child*>(children[pos].ref());
		if (c)
			c->set_parent(element_ptr());
		children.erase(children.begin()+pos, children.begin()+pos+1); 
	}
	/// insert a child at a given position
	virtual void on_swap_children(unsigned int p1, unsigned int p2) 
	{
		std::swap(children[p1],children[p2]);
	}
	/// set the group options
	virtual void on_set_options(options _o)
	{
		o = _o;
	}
public:
	/// construct button from name, label text, parent, and visible and active flags
	group(const std::string& _name, const std::string& _text, element_ptr _parent = element_ptr(), bool _visible = true) 
		: label(_name, _text, _parent, _visible) {
			insert_child.attach_method(this, &group::on_insert_child, _0, _1);
			remove_child.attach_method(this, &group::on_remove_child, _0);
			swap_children.attach_method(this, &group::on_swap_children, _0, _1);
			set_options.attach_method(this, &group::on_set_options, _0);
			o = VERTICAL;
			set_placement(TOP_LABEL);
	}
	/// access to children
	element_ptr get_child(unsigned int i) const { return children[i]; }
	/// return the current number of children
	unsigned int get_nr_children() const { return (unsigned int) children.size(); }
	/// signal used to insert a child at a given position
	utils::signal<unsigned int, element_ptr> insert_child;
	/// signal used to remove a child at a given position
	utils::signal<unsigned int> remove_child;
	/// signal used to swap two children
	utils::signal<unsigned int, unsigned int> swap_children;

	/// shortcut to insert a child at the end
	void add_child(element_ptr e) { insert_child( get_nr_children(), e); }
	/// shortcut to remove a child
	void del_child(element_ptr e) {
		std::vector<element_ptr>::iterator i = std::find(children.begin(), children.end(), e);
		if (i != children.end())
			remove_child( (int)(i-children.begin()));
	}

	/// query current group options
	options get_options() const { return o; }
	/// signal used to set options
	utils::signal<options>  set_options;
	/// shortcut to add options
	void add_options(options _o) { set_options((options)(get_options() | _o)); }
	/// shortcut to remove options
	void del_options(options _o) { set_options((options)(get_options()& ~_o)); }


	/// add a new button, attach select to its press signal and return pointer to button
	button_ptr add_button(const std::string& _text, const std::string& _label)  { 
		button_ptr b = button_ptr(new button(_text, _label, element_ptr(this))); 
		add_child(element_ptr(b.ref())); 
		return b; 
	}
	/// insert a new button at a given position, attach select to its press signal and return pointer to the button
	button_ptr insert_button(unsigned int pos, const std::string& _text, const std::string& _label) { 
		button_ptr b = button_ptr(new button(_text, _label, element_ptr(this))); 
		insert_child(pos, element_ptr(b.ref())); 
		return b; 
	}
};

/// define reference counted pointer to groups
typedef storage::ref_ptr<group> group_ptr;

/// query the number of children in a group element, if it is not derived from group return 0
inline unsigned int get_nr_children(const element* e)
{
	const group* g = dynamic_cast<const group*>(e);
	if (g)
		return g->get_nr_children();
	return 0;
}

	}
}