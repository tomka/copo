#include <cgv/gui/fltk/osg/Viewer.h>

#include <fltk/ask.h>
#include <fltk/events.h>
#include <fltk/run.h>
#include <fltk/file_chooser.h>
#include <fltk/filename.h>
#include <fltk/Item.h>
#include <fltk/ThumbWheel.h>
#include <fltk/Button.h>
#include <fltk/ItemGroup.h>
#include <fltk/TiledGroup.h>
#include <fltk/Divider.h>
#include <fltk/MenuBar.h>
#include <fltk/PopupMenu.h>
#include <fltk/Color.h>
#include <cgv/gui/fltk/osg/MatrixTransformUI.h>
#include <osgDB/ReadFile>
#include <osg/Geode>
#include <osgUtil/GLObjectsVisitor>
#include <cgv/registry/registry.h>
#include <cgv/gui/fltk/osg/CoordinateGrid.h>

namespace cgv {
	namespace gui {
		namespace fltk {
			namespace osg {

void quit_cb(::fltk::Widget*, void*)
{
	exit(0);
}

void Viewer::timer_cb()
{
	::fltk::redraw();
	if (animate_item->value())
		::fltk::repeat_timeout(1.0/fps,timer_cb,this);
}

void Viewer::toggle_animate_cb()
{
	std::cout << "toggle animate" << std::endl;
	if (animate_item->value())
		::fltk::add_timeout(1.0/fps,timer_cb,this);
}

using namespace ::fltk;

void Viewer::fps_cb()
{
	char buffer[500];
	sprintf(buffer,"%i", fps);
	const char* text = input("please enter a new frame rate", buffer);
	float new_fps;
	if (sscanf(text,"%f", &new_fps)) {
		fps = new_fps;
		sprintf(buffer,"%i fps", fps);
		fps_item->copy_label(buffer);
	}
}

void Viewer::select_scene(Window* osg_window)
{

	osg_tab_group->selected_child(osg_window);
	osg_tab_group->resizable(osg_window);
	::cgv::gui::fltk::osg::Window *w =(::cgv::gui::fltk::osg::Window *)osg_window;
	wireframe_item->value(w->getWireFrameOn());
	grid_item->value(w->getGridOn());
	
	
	::fltk::Browser* gui_browser = (::fltk::Browser*) osg_window->user_data();
	for (int i = 0; i < gui_scroll_group->children(); ++i) {
		::fltk::Widget* child = gui_scroll_group->child(i);
		if (child == gui_browser) {
			child->show();
			child->callback(browser_cb,this);
		}
		else {
			child->hide();
			child->callback((Callback*)0);
		}
	}
}

void Viewer::open_cb()
{
	const char *file_path = file_chooser("Open an OpenSceneGraph file.","*.osg","",1);
	if (!file_path)
		return;
	std::string file_path_s(file_path);
	// load the scene.
	::osg::Node* loaded_scene = osgDB::readNodeFile(file_path_s);
	if (!loaded_scene)
		return;

	add_scene(loaded_scene, file_path);
}

void Viewer::osg_tab_change_cb()
{
	::fltk::Widget* child = osg_tab_group->selected_child();
	if (child == empty_osg_widget)
		return;
	select_scene((Window*) child);
}

void Viewer::save_cb()
{
	std::cout << "save" << std::endl;
}
void Viewer::close_cb()
{
	::fltk::Widget* child = osg_tab_group->selected_child();
	if (child == empty_osg_widget)
		return;
	::fltk::Browser* gui_browser = (::fltk::Browser*) child->user_data();
	gui_scroll_group->remove(gui_browser);
	delete gui_browser;
	osg_tab_group->remove(child);
	if (osg_tab_group->children() == 0) {
		empty_osg_widget = new ::fltk::Widget(child->x(),child->y(),child->w(),child->h(),"empty, press Crtl-O to open scene");
		empty_osg_widget->style(foreground_style);
		osg_tab_group->add(empty_osg_widget);
		osg_tab_group->resizable(empty_osg_widget);
	}
	else {
		osg_tab_group->value();
		select_scene((Window*)osg_tab_group->selected_child());
	}
	delete child;
}
void Viewer::wheel_cb()
{
	std::cout << "wheel" << std::endl;
}
void Viewer::wireframe_cb()
{
	cgv::gui::fltk::osg::Window* osg_window =  (cgv::gui::fltk::osg::Window*)osg_tab_group->selected_child();
	
	osg_window->setWireFrameOn(wireframe_item->value());
	


	std::cout << "turned wireframe " << (wireframe_item->value()?"on":"off") << std::endl;
}
void Viewer::grid_cb()
{
	cgv::gui::fltk::osg::Window* osg_window =  (cgv::gui::fltk::osg::Window*)osg_tab_group->selected_child();
	
	osg_window->setGridOn(grid_item->value());
	std::cout << "turned grid " << (grid_item->value()?"on":"off") << std::endl;
}

void Viewer::flat_cb()
{
	std::cout << "flat" << std::endl;
}
void Viewer::gouraud_cb()
{
	std::cout << "gouraud" << std::endl;
}
void Viewer::phong_cb()
{
	std::cout << "phong" << std::endl;
}

void Viewer::browser_cb(::fltk::Browser* browser)
{
	// extract dimensions of property tab element widget
	::fltk::Widget* sw = gui_tab_group->selected_child();
	int x = sw->x(),y = sw->y(), w = sw->w(), h = sw->h();
	// extract pointer to node from browser
	::fltk::Widget* item_w = browser->item();
	::osg::Node* node = (::osg::Node*)item_w->user_data();
	// remove previous user interface
	gui_tab_group->remove_all();
	if (empty_property_widget) {
		delete empty_property_widget;
		empty_property_widget = 0;
	}
	if (node_ui) {
		delete node_ui;
		node_ui = 0;
	}
	// construct new user interface
	gui_tab_group->begin();
	const cgv::registry::factory_vector_type* factories = cgv::registry::find(node->className(),"gui::fltk::osg::NodeUI");
	if (factories) {
		node_ui = (NodeUI*) cgv::registry::construct(factories->front(),x,y,w,h,factories->front()->get_name());
		node_ui->node(node);
		node_ui->integrate(*gui_tab_group);
	}
	else {
		empty_property_widget = new ::fltk::Widget(x,y,w,h,"no UI defined");
		gui_tab_group->add(empty_property_widget);
		empty_property_widget->style(foreground_style);
		gui_tab_group->resizable(empty_property_widget);
	}
	gui_tab_group->end();
}

/// access to current browser
::fltk::Browser* Viewer::get_current_browser() const
{
	// not yet implemented
	return 0;
}

Viewer::Viewer(const char* label) : Window(USEDEFAULT,USEDEFAULT,1024,600,label,true)
{
	fps = 25;
	node_ui = 0;
	empty_osg_widget = 0;
	empty_property_widget = 0;
	setStyles();
	buildMenu();
	TiledGroup* tile = new ::fltk::TiledGroup(0,30,w(),h()-30);
	tile->begin();
		int ww = w()/2; int wh = tile->h();
		build_osg_tab_group(0,0,ww,wh);
		tile->resizable(osg_tab_group);
		build_gui_tab_group(ww,0,ww,wh/2);
		build_gui_scroll_group(ww,wh/2,ww,wh/2);
	tile->end();
	resizable(tile);
	size_range(400,250);
	end();
	toggle_animate_cb();
}

/// overload show methods to avoid direct reference to fltk
void Viewer::show()
{
	::fltk::Window::show();
}

void Viewer::show(int argc, char** argv)
{
	::fltk::Window::show(argc,argv);
}




void Viewer::add_scene(::osg::Node* root_node, const char* file_path)
{
	const ::fltk::Widget* widget = osg_tab_group->selected_child();
	cgv::gui::fltk::osg::Window* osg_window = new cgv::gui::fltk::osg::Window(widget->x(),widget->y(),widget->w(),widget->h());
	osg_window->copy_label(filename_name(file_path));
	
	
	
	osg_window->setSceneData(root_node);
	
	
	osg_window->style(foreground_style);
	osg_tab_group->add(osg_window);
	if (empty_osg_widget) {
		osg_tab_group->remove(empty_osg_widget);
		empty_osg_widget = 0;
	}
	::fltk::Browser* gui_browser = construct_gui_browser(root_node);
	gui_scroll_group->add(gui_browser);
	osg_window->user_data(gui_browser);
	select_scene(osg_window);
}

void Viewer::setStyles()
{
	// different styles
	menu_style.parent_ = style();
	menu_style.color(GRAY80);
	
	menu_style.textcolor(::fltk::color(0,0,80));
	menu_style.box(::fltk::UP_BOX);
	menu_style.buttonbox(::fltk::HIGHLIGHT_DOWN_BOX);

	foreground_style.parent_ = style();
	foreground_style.textcolor(::fltk::color(0,50,0));
	foreground_style.color(::fltk::color(220,240,220));
	foreground_style.selection_color(::fltk::color(200,255,200));

	background_style.parent_ = style();
	background_style.textcolor(::fltk::color(100,0,0));
	background_style.color(::fltk::color(220,220,255));
}


void Viewer::buildMenu()
{
	::fltk::MenuBar* menubar = new ::fltk::MenuBar(0,0,w(),24);
	menubar->style(&menu_style);
	menubar->begin();
		::fltk::ItemGroup* group = new ::fltk::ItemGroup("&File");
		group->begin();
			::fltk::Item* item = new ::fltk::Item(" Open");
			item->shortcut(COMMAND+'o');
			item->callback(open_cb, this);
			item = new ::fltk::Item("Save");
			item->shortcut(COMMAND+'s');
			item->callback(save_cb, this);
			item = new ::fltk::Item("Close");
			item->callback(close_cb, this);
			new Divider();
			item = new ::fltk::Item("Quit");
			item->shortcut(COMMAND+'q');
			item->callback(quit_cb, this);
		group->end();

		group = new ::fltk::ItemGroup("&View");
		group->begin();
			::fltk::ItemGroup* group_1 = new ::fltk::ItemGroup("&Shading");
			group_1->begin();
				wireframe_item = new ::fltk::Item("Wireframe");
				wireframe_item->type(::fltk::Item::TOGGLE);
				wireframe_item->callback(wireframe_cb, this);
				grid_item = new ::fltk::Item("Grid");
				grid_item->type(::fltk::Item::TOGGLE);
				grid_item->value(true);
				grid_item->callback(grid_cb, this);
				item = new ::fltk::Item("Flat");
				item->type(::fltk::Item::RADIO);
				item->callback(flat_cb, this);
				item = new ::fltk::Item("Gouraud");
				item->type(::fltk::Item::RADIO);
				item->callback(gouraud_cb, this);
				item = new ::fltk::Item("Phong");
				item->type(::fltk::Item::RADIO);
				item->callback(phong_cb, this);

			group_1->end();
		group->end();

		group = new ::fltk::ItemGroup("&Animation");
		group->begin();
			animate_item = new ::fltk::Item("Animate");
			animate_item->type(::fltk::Item::TOGGLE);
			animate_item->callback(toggle_animate_cb, this);
			animate_item->value(true);
			fps_item = new ::fltk::Item("25 fps");
			fps_item->callback(fps_cb, this);
		group->end();

	menubar->end();
}

void Viewer::build_osg_tab_group(int x, int y, int w, int h, int ww, int bw)
{
	::fltk::Group* group = new ::fltk::Group(x,y,w,h);
	group->box(DOWN_BOX);
	group->set_flag(ALIGN_CLIP);
	group->style(background_style);
	group->begin();
		osg_tab_group = new ::fltk::TabGroup(2*bw+ww,bw,w-3*bw-ww,h-3*bw-ww);
		osg_tab_group->style(foreground_style);
		osg_tab_group->set_flag(ALIGN_CLIP);
		osg_tab_group->callback(osg_tab_change_cb, this);
		group->resizable(osg_tab_group);     // this ensures that only the osg_window is resized

		osg_tab_group->begin();
			empty_osg_widget = new ::fltk::Widget(0,ww,w-3*bw-ww,h-3*bw-2*ww,"empty, press Crtl-O to open scene");
			empty_osg_widget->style(foreground_style);
			osg_tab_group->resizable(empty_osg_widget);
		osg_tab_group->end();

		::fltk::ThumbWheel* wheel = new ::fltk::ThumbWheel(2*bw+ww,h-bw-ww, w-3*bw-ww,ww);
		wheel->set_flag(ALIGN_CLIP);
		wheel->callback(wheel_cb, this);
		wheel->style(foreground_style);

		wheel = new ::fltk::ThumbWheel(bw,bw+ww,ww,h-3*bw-2*ww);
		wheel->set_vertical();
		wheel->set_flag(ALIGN_CLIP);
		wheel->callback(wheel_cb, this);
		wheel->style(foreground_style);
	group->end();
	group->resizable(osg_tab_group);
}

void Viewer::build_gui_tab_group(int x, int y, int w, int h, int ww, int bw)
{
	::fltk::Group* group = new ::fltk::Group(x,y,w,h);
	group->box(DOWN_BOX);
	group->set_flag(ALIGN_CLIP);
	group->style(background_style);
	group->begin();
		gui_tab_group = new ::fltk::TabGroup(bw,ww,w-2*bw,h-ww-bw,"Property Tabs");
		gui_tab_group->style(foreground_style);
		gui_tab_group->begin();
			empty_property_widget = new ::fltk::Widget(0,ww,w-2*bw,h-2*ww-bw,"no UI defined");
			empty_property_widget->style(foreground_style);
			gui_tab_group->resizable(empty_property_widget);
		gui_tab_group->end();
	group->end();
	group->resizable(gui_tab_group);
}

void Viewer::build_gui_scroll_group(int x, int y, int w, int h, int ww, int bw)
{
	::fltk::Group* group = new ::fltk::Group(x,y,w,h);
	group->box(DOWN_BOX);
	group->set_flag(ALIGN_CLIP);
	group->style(background_style);
	group->begin();
		gui_scroll_group = new ScrollGroup(bw,ww,w-2*bw,h-ww-bw,"Scene Graph ::fltk::Browser");
		gui_scroll_group->style(foreground_style);
	group->end();
	group->resizable(gui_scroll_group);
}


::fltk::Browser*  Viewer::construct_gui_browser(::osg::Node* scene)
{
	::fltk::Browser* gui_browser = new ::fltk::Browser(0,0,600,600);
	const char *labels[] = {"Name", "Type", "Library", 0};
	int widths[]   = {100, 100, 100, 0};
	gui_browser->column_widths(widths);
	gui_browser->column_labels(labels);
	gui_browser->indented(3);
	gui_browser->when(WHEN_CHANGED);
	add_node_to_gui_browser(gui_browser, scene, gui_browser);
	return gui_browser;
}

void Viewer::add_state_set_to_gui_browser(::fltk::Browser* gui_browser, ::osg::StateSet* state_set, ::fltk::Group* parent)
{
	::fltk::Item* i = gui_browser->add_leaf("state set", parent);
	i->user_data(state_set);
}

void Viewer::add_drawable_to_gui_browser(::fltk::Browser* gui_browser, ::osg::Drawable* drawable, ::fltk::Group* parent)
{
	::fltk::Item* i = gui_browser->add_leaf(drawable->className(), parent);
	i->user_data(drawable);
}

void Viewer::add_node_to_gui_browser(::fltk::Browser* gui_browser, ::osg::Node* node, ::fltk::Group* parent)
{	
	std::string* label_ptr = new std::string(node->getName());
	std::string& label = *label_ptr;
	if (label.empty())
		label += "not named";
	label += "\t";
	label += node->className();
	label += "\t";
	label += node->libraryName();
	::fltk::ItemGroup* g = 0;
	if (node->getStateSet()) {
		g = gui_browser->add_group(label.c_str(), parent, OPENED);
		g->user_data(node);
		add_state_set_to_gui_browser(gui_browser, node->getStateSet(), g);
	}
	::osg::Group* group = node->asGroup();
	if (group) {
		if (!g) {
			g = gui_browser->add_group(label.c_str(), parent);
			g->user_data(node);
		}
		for (unsigned int i = 0; i < group->getNumChildren(); ++i)
			add_node_to_gui_browser(gui_browser, group->getChild(i), g);
		return;
	}
	if (strcmp(node->className(),"Geode") == 0) {
		::osg::Geode* geode = (::osg::Geode*) node;
		std::cout << "identified Geode with " << std::endl;
		if (geode->getNumDrawables() > 0) {
			if (!g) {
				g = gui_browser->add_group(label.c_str(), parent, OPENED);
				g->user_data(node);
			}
			for (unsigned int i = 0; i < geode->getNumDrawables(); ++i)
				add_drawable_to_gui_browser(gui_browser, geode->getDrawable(i), g);
			return;
		}
	}
	::fltk::Item* i = gui_browser->add_leaf(label.c_str(), parent);
	i->user_data(node);
}

			}
		}
	}
}
