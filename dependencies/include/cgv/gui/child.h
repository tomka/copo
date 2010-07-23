#pragma once

#include <string>
#include <cgv/gui/element.h>

namespace cgv {
	namespace gui {

using namespace utils::Signal;

/** base for all gui classes that have a parent */
class child : public element
{
protected:
	/// pointer to parent
	element_ptr parent;
	/// internal callback to set the parent of an element
	virtual void on_set_parent(element_ptr _parent) { parent = _parent; }
public:
	/// all elements are named, by default visible and reference counted
	child(const std::string& _name, element_ptr _parent = element_ptr())
		: element(_name), parent(_parent) {
			set_parent.attach_method(this, &child::on_set_parent, _0);
	}
	/// return the parent
	element_ptr get_parent() const					{ return parent; }
	/// set a different parent
	utils::signal<element_ptr> set_parent;
};

/// define reference counted pointer to child elements
typedef storage::ref_ptr<child> child_ptr;

/// query the parent of an element, if it is not derived from child return empty element_ptr
inline element_ptr get_parent(const element* e)
{
	const child* c = dynamic_cast<const child*>(e);
	if (c)
		return c->get_parent();
	return element_ptr();
}


	}
}