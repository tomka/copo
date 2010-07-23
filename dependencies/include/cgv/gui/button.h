#pragma once

#include <cgv/utils/signal.h>
#include <cgv/gui/label.h>

namespace cgv {
	namespace gui {

using namespace utils;

/// a button is labeled and can be pressed
class button : public label
{
public:
	/// construct button from name, label text, parent, and visible and active flags
	button(const std::string& _name, const std::string& _text, element_ptr _parent = element_ptr(), bool _visible = true, bool _active = true) 
		: label(_name, _text, _parent, _visible, _active) 
	{
		set_placement(INSIDE_LABEL);
	}
	/// signal called when button is press
	utils::signal<> press;
};

/// define reference counted pointer to button
typedef storage::ref_ptr<button> button_ptr;

	}
}
