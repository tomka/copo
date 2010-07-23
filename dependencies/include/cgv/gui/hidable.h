#pragma once

#include <string>
#include <cgv/storage/ref_ptr.h>
#include <cgv/gui/child.h>

namespace cgv {
	namespace gui {

/** base for all gui classes that have the visible flag */
class hidable : public child
{
protected:
	/// store whether element is visible
	bool visible;
	/// internal set_visible callback
	virtual void on_set_visible(bool _visible) { visible = _visible; }
public:
	/// construct from name, parent, and visible flag
	hidable(const std::string& _name, element_ptr _parent = element_ptr(), bool _visible = true) 
		: child(_name, _parent), visible(_visible) {
			set_visible.attach_method(this, &hidable::on_set_visible, _0);
	}
	/// return whether hidable is visible
	bool is_visible() const										{ return visible; }
	/// signal used to change visibility state
	utils::signal<bool> set_visible;
	/// shortcut to make invisible
	void hide()						        { set_visible(false); }
	/// shortcut to make visible
	void show()						        { set_visible(true); }
};

/// define reference counted pointer to hidables
typedef storage::ref_ptr<hidable> hidable_ptr;

/// recursive query if an element is visible: check recursively if parent is visible and if hidale is visible
inline bool is_visible(const element* e)
{
	const hidable* h = dynamic_cast<const hidable*>(e);
	if (h)
		if (!h->is_visible())
			return false;

	element_ptr p = get_parent(e);
	if (!p.empty())
		if (!is_visible(p.ref()))
			return false;

	return true;
}

	}
}