#pragma once

#include <string>
#include <cgv/storage/ref_ptr.h>
#include <cgv/gui/hidable.h>

namespace cgv {
	namespace gui {

/** base for all gui class that have the active flag */
class actable : public hidable
{
protected:
	/// whether the actable is active
	bool active;
	/// internal set_text callback
	virtual void on_set_active(bool _active) { active = _active; }
public:
	/// construct from name, parent, and visible and active flags
	actable(const std::string& _name, element_ptr _parent = element_ptr(), bool _visible = true, bool _active = true) 
		: hidable(_name, _parent, _visible), active(_active) {
			set_active.attach_method(this, &actable::on_set_active, _0);
	}
	/// return whether element is active
	bool is_active() const					{ return active; }
	/// signal used to change the active state
	utils::signal<bool> set_active;
};

/// define reference counted pointer to actables
typedef storage::ref_ptr<actable> actable_ptr;

/// recursive query if an element is active: check recursively if parent is active and if actable is active
inline bool is_active(const element* e)
{
	const actable* a = dynamic_cast<const actable*>(e);
	if (a)
		if (!a->is_active())
			return false;

	element_ptr p = get_parent(e);
	if (!p.empty())
		if (!is_active(p.ref()))
			return false;

	return true;
}

	}
}