#include <cgv/gui/fltk/gui.h>
#include <cgv/gui/fltk/manager.h>
#include <cgv/gui/fltk/fltk_manager.h>

namespace cgv {
	namespace gui {
		namespace fltk {

/// construct empty gui
manager::manager(group_ptr _app)
{
	implementation = new fltk_manager(_app);
}

/// virtual destructor
manager::~manager()
{
	delete (fltk_manager*) implementation;
}

/// attach gui manager to application group
void manager::attach(group_ptr m)
{
	((fltk_manager*)implementation)->attach(m);
}
/// detach gui manager from application group
void manager::detach()
{
	((fltk_manager*)implementation)->detach();
}
void manager::run(int argc, char** argv)
{
	((fltk_manager*)implementation)->run(argc, argv);
}

void manager::terminate()
{
	((fltk_manager*)implementation)->terminate();
}
/// configure the gui manager from a text that describes properties of gui elements by name
void manager::configure(const std::string& text)
{
	((fltk_manager*)implementation)->configure(text);
}

		}
	}
}