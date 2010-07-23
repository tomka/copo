#pragma once

#include <string>
#include <cgv/storage/ref_ptr.h>
#include <cgv/gui/actable.h>

namespace cgv {
	namespace gui {

/** base for all gui class that have a text string */
class label : public actable
{
public:
	/// different placement modes
	enum placement_mode { NO_LABEL, LEFT_LABEL, RIGHT_LABEL, BOTTOM_LABEL, TOP_LABEL, INSIDE_LABEL };
protected:
	/// store the label text
	std::string text;
	/// internal set_text callback
	virtual void on_set_text(const std::string& _text) { text = _text; }
	/// store the placement mode
	placement_mode placement;
	/// internal set_placement callback
	virtual void on_set_placement(placement_mode _placement) { placement = _placement; }
public:
	/// construct from name, label text, parent, and visible and active flags
	label(const std::string& _name, const std::string& _text, element_ptr _parent = element_ptr(), bool _visible = true, bool _active = true) 
		: actable(_name, _parent, _visible, active), text(_text) {
			set_text.attach_method(this, &label::on_set_text, _0);
			set_placement.attach_method(this, &label::on_set_placement, _0);
			placement = LEFT_LABEL;
	}
	/// return the text of the label
	const std::string& get_text() const					{ return text; }
	/// return the placement mode
	placement_mode get_placement() const                { return placement; }
	/// signal used to change the label text
	utils::signal<const std::string&> set_text;
	/// signal used to change the label placement
	utils::signal<placement_mode> set_placement;
};

/// define reference counted pointer to labels
typedef storage::ref_ptr<label> label_ptr;

/// query the label text of an element 
inline std::string get_text(const element* e)
{
	const label* l = dynamic_cast<const label*>(e);
	if (l)
		return l->get_text();
	return std::string();
}

	}
}