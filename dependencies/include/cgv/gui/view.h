#pragma once

#include <cgv/gui/label.h>
#include <cgv/gui/observer.h>
#include <cgv/storage/ref_ptr.h>

namespace cgv {
	namespace gui {

/** abst interface extension of all view classes. */
class abst_view
{
public:
	/// construct button from name
	abst_view() {}
	/// return the abst_observer
	virtual abst_observer_ptr get_observer()             = 0;
	virtual const abst_observer* get_observer() const = 0;
};

/** a view shows a value stored in an observed. */
template <typename T>
class view : public observer<T>, public abst_view
{
public:
	/// construct button from name
	view(const std::string& _name, const std::string& _text, element_ptr _parent = element_ptr(), bool _visible = true, bool _active = false)
		: observer<T>(_name, _text, _parent, _visible, _active) {
	}
	abst_observer_ptr get_observer()          { return abst_observer_ptr(this); }
	const abst_observer* get_observer() const { return this; }
};

typedef storage::ref_ptr<view<bool> >        bool_view_ptr;
typedef storage::ref_ptr<view<int> >         int_view_ptr;
typedef storage::ref_ptr<view<float> >       float_view_ptr;
typedef storage::ref_ptr<view<std::string> > string_view_ptr;

	}
}