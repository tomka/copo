#pragma once

#include <cgv/gui/label.h>
#include <cgv/gui/observer.h>
#include <cgv/storage/ref_ptr.h>

namespace cgv {
	namespace gui {

/** abst interface extension of all edit classes. */
class abst_editor
{
public:
	/// construct
	abst_editor() {}
	/// return the abst_observer
	virtual abst_observer_ptr abst_get_observer()             = 0;
	virtual const abst_observer* abst_get_observer() const = 0;
};

/** an edit shows a value stored in an observed. */
template <typename T>
class editor : public observer<T>, public abst_editor
{
public:
	/// construct edit from name
	editor(const std::string& _name, const std::string& _text, element_ptr _parent = element_ptr(), bool _visible = true, bool _active = false)
		: observer<T>(_name, _text, _parent, _visible, _active) {
	}
	/// return the abst_observer
	abst_observer_ptr abst_get_observer()          { return abst_observer_ptr(this); }
	const abst_observer* abst_get_observer() const { return this; }
};

typedef storage::ref_ptr<editor<bool> >        bool_editor_ptr;
typedef storage::ref_ptr<editor<int> >         int_editor_ptr;
typedef storage::ref_ptr<editor<std::string> > string_editor_ptr;

	}
}