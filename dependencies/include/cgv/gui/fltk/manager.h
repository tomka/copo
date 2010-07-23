#pragma once

#include <cgv/gui/abst_manager.h>

#include <cgv/config/global.h>
#include <cgv/gui/fltk/lib_begin.h>

#pragma warning (disable:4275)

namespace cgv {
	namespace gui {
		namespace fltk {

/** manage a gui with fltk */
class CGV_API manager : public abst_manager
{
protected:
	/// used to store the implementation of the manager
	void* implementation;
public:
	/// construct empty gui
	manager(group_ptr _app = group_ptr());
	/// virtual destructor
	~manager();
	/// attach gui manager to application, which can be a group of windows or a single window
	void attach(group_ptr m);
	/// detach gui manager from application group
	void detach();
	/// start the main loop of the manager
	void run(int argc, char** argv);
	/// terminate the gui manager
	void terminate();
	/// configure the gui manager from a text that describes properties of gui elements by name
	void configure(const std::string& text);
};

		}
	}
}