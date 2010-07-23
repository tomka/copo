#pragma once

#include <string>
#include <cgv/utils/signal.h>
#include <cgv/storage/ref_ptr.h>
#include <cgv/storage/ref_counted.h>

namespace cgv {
	namespace gui {

class abst_gui;

using namespace utils::Signal;

/** base class for all gui classes */
class element : public storage::ref_counted
{
protected:
	/// name of element
	std::string name;
	/// internal callback to set the element name
	virtual void on_set_name(const std::string& _name) { name = _name; }
public:
	/// all elements are named, by default visible and reference counted
	element(const std::string& _name)
		: name(_name) {
			set_name.attach_method(this, &element::on_set_name, _0);
	}
	/// define virtual destructor
	virtual ~element()								{}
	/// return the element name
	const std::string& get_name() const				{ return name; }
	/// signal used to change the element name
	utils::signal<const std::string&> set_name;
};

/// define reference counted pointer to elements
typedef storage::ref_ptr<element> element_ptr;

	}
}