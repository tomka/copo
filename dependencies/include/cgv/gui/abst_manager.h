#pragma once

#include <cgv/gui/group.h>

namespace cgv {
	namespace gui {

/** base class of all gui manager implementations */
class abst_manager
{
public:
	/// virtual destructor
	virtual ~abst_manager() {}
	/// attach gui manager to application, which can be a group of windows or a single window
	virtual void attach(group_ptr m) = 0;
	/// detach gui manager from application group
	virtual void detach() = 0;
	/// start the main loop of the manager
	virtual void run(int argc, char** argv) = 0;
	/// terminate the gui manager
	virtual void terminate() = 0;
	/// configure the gui manager from a text that describes properties of gui elements by name
	virtual void configure(const std::string& text) {}
};

	}
}