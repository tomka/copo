#include <cgv/scene/gui/gl_view_base.h>
#include <cgv/scene/graph/group_node.h>
#include "controller.h"

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>


namespace cgv{
	namespace scene{
		namespace gui{

class gl_view_scene : public gl_view_base
{
protected:

	
	cgv::scene::graph::node *scene;
	controller * ctrl;


public:
  
	///constructor
	gl_view_scene(int x,int y,int w,int h): gl_view_base(x,y,w,h)
	{
		
		scene=NULL;
		ctrl=NULL;
		
	}   
	///destructor
	virtual ~gl_view_scene(){}

	

public:

	

	void setup_scene(cgv::scene::graph::node* root)
	{
		init_ok=false;
		if(root)
			scene = root;
		
	}

	void setup_controller(controller* eventcontroller)
	{
		if(eventcontroller)
			ctrl=eventcontroller;
	}


	
	
	virtual void mouse_down(int x, int y,int button)
	{
		if(ctrl)
			ctrl->mouse_down(x,y,button);
		update(true);
		
	} 

	virtual void mouse_move(int x, int y)
	{
		

		if(ctrl)
			ctrl->mouse_move(x,y);
	
		update(true);
	}

	virtual void mouse_up(int x, int y,int button) 
	{
		
		if(ctrl)
			ctrl->mouse_up(x,y,button);
	
		update(true);
	}

	virtual void mouse_wheel(int dy)
	{
		
		if(ctrl)
			ctrl->mouse_wheel(dy);
	
		update(true);

	}

	virtual void key_down(unsigned int key)
	{
		
		if(ctrl)
			ctrl->key_down(key);
	
		update(true);
	}
	
	void reshape(int w, int h)
	{
		if(ctrl)
			ctrl->reshape(w,h);
		 
	}

	void init()
	{
		glClearColor(0.7f,0.7f,0.8f,1.0f);
		glShadeModel(GL_SMOOTH);							
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_NORMALIZE);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		
		if(scene)
			scene->init();

	}

	void render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);      
		glLoadIdentity(); 
		
		
		if(scene &&init_ok)
			scene->render();

	}
	
	



};

		}
	}
}

