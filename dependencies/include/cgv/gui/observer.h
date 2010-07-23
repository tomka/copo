#pragma once

#include <cgv/gui/label.h>
#include <cgv/gui/observed.h>
#include <cgv/storage/ref_ptr.h>

namespace cgv {
	namespace gui {

/** abstract interface to all observers of observeds. */
class abst_observer : public label
{
protected:
	/// internal update callback
	virtual void on_update() { }
public:
	/// construct button from name
	abst_observer(const std::string& _name, const std::string& _text, element_ptr _parent = element_ptr(), bool _visible = true, bool _active = true) 
		: label(_name, _text, _parent, _visible, _active) {
			update.attach_method(this, &abst_observer::on_update);
	}
	/// access to observed
	virtual abst_observed_ptr abst_get_observed() const = 0;
	/// set new observed
	virtual void abst_set_observed(abst_observed_ptr) = 0;
	/// signal triggered when value of observed has been changed
	utils::signal<> update;
};

typedef storage::ref_ptr<abst_observer>				abst_observer_ptr;

/** an observer stores a pointer to an observed and receives update events. */
template <typename T>
class observer : public abst_observer
{
	/// reference counted pointer to observed
	typedef storage::ref_ptr<observed_base<T> > observed_ptr;
protected:
	/// keep reference counted pointer to observed
	observed_ptr obs;
	/// internal callback to set the observed
	virtual void on_set_observed(observed_ptr _obs)
	{
		if (!obs.empty())
			obs->set_value.detach_instance(update);
		obs = _obs;
		obs->set_value.attach_instance(update); 
	}
public:
	/// construct button from name
	observer(const std::string& _name, const std::string& _text, element_ptr _parent = element_ptr(), bool _visible = true, bool _active = false)
		: abst_observer(_name, _text, _parent, _visible, _active) {
			set_observed.attach_method(this, &observer<T>::on_set_observed, _0);
	}
	/// return the observed
	observed_ptr get_observed() const           { return obs; }
	/// signal used to set a new observed
	utils::signal<observed_ptr> set_observed;
	/// access to observed
	abst_observed_ptr abst_get_observed() const { return abst_observed_ptr(obs.ref()); }
	/// set new observed
	void abst_set_observed(abst_observed_ptr o) { set_observed(observed_ptr(dynamic_cast<observed_base<T>*>(o.ref()))); }
};

typedef storage::ref_ptr<observer<bool> >        bool_observer_ptr;
typedef storage::ref_ptr<observer<int> >         int_observer_ptr;
typedef storage::ref_ptr<observer<float> >       float_observer_ptr;
typedef storage::ref_ptr<observer<std::string> > string_observer_ptr;

	}
}