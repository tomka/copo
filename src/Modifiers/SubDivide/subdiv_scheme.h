#pragma once

#include "../../polygonal_mesh.h"
#include <fltk/Group.h>
#include <fltk/CheckButton.h>

/**
* Base class for all subdivision implementations.
*/
class subdiv_scheme : public fltk::Group {
protected:
	/// current subdivision level / subdivision step
	int step;
	/// whether steps are printed out
	bool debugSteps;
	/// checkbutton for debugSteps
	fltk::CheckButton *checkbutton_1;
	/// halfedge
	typedef SnowSim::polygonal_mesh::halfedge halfedge;
	/// edge
	typedef SnowSim::polygonal_mesh::edge edge;
	/// vertex
	typedef SnowSim::polygonal_mesh::vertex vertex;
	/// face
	typedef SnowSim::polygonal_mesh::face face;
public:
	/// constructor
	subdiv_scheme(int x, int y, int width, int height)
		:	fltk::Group(x,y,width,height,"------------- settings ------------",true)
	{
		step = 0;
		debugSteps = false;
		checkbutton_1 = new fltk::CheckButton(0,0,20,20,"debug steps");
		checkbutton_1->value(debugSteps);
		checkbutton_1->callback(checkbutton_1_cb,this);
		end();
	}
	/// destructor
	~subdiv_scheme() {
		delete checkbutton_1;
	}
	/// get name of scheme
	virtual const char* get_name() const {
		return "generic";
	}
	/// calls the subdiv_step function (subdiv_steps times)
	virtual void subdivide(SnowSim::polygonal_mesh* mesh, int subdiv_steps) {
		if (debugSteps) {
			std::cout << "----------------- subdiv " << get_name() << " ------------" << std::endl;
		}
		for (step = 1; step <= subdiv_steps; step++) {
			if (debugSteps) {
				std::cout << "------------- step " << step << " ------- "<< std::endl;
			}
			subdiv_step(mesh);
		}
	}
	/// subdivision step
	virtual void subdiv_step(SnowSim::polygonal_mesh* mesh) {}

	/// static callback for debugSteps checkbutton
	static inline void checkbutton_1_cb(fltk::Widget *w,void *data)
	{
		((subdiv_scheme*)data)->checkbutton_1_cb();
	}
	/// callback for debugSteps checkbutton
	void checkbutton_1_cb()
	{
		debugSteps = checkbutton_1->value();
		do_callback();
	}
};
