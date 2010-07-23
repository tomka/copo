#pragma once

#include <cgv/utils/observer.h>
#pragma warning (disable:4311)
#include <fltk/run.h>
#include <fltk/GlWindow.h>
#include <fltk/events.h>
#include <iostream>
#include <vector>


namespace cgv{
	namespace scene{
		namespace gui{


class gl_view_base : public ::fltk::GlWindow, public cgv::utils::observer
{
public:	
	bool init_ok;	
	bool animation_on;
	int fps;

	
	
	void draw()
	{
		if(!valid()|| !init_ok)
		{
			if(!init_ok)
			{
				init();
				init_ok=true;
			}
			reshape(w(),h());
		}
		render();
	}
	
	int handle(int event) 
	{
		int mx ,my,dy;
		unsigned int key,button;
				
		switch(event) 
		{
		
			case ::fltk::PUSH:
				mx = ::fltk::event_x();
				my = ::fltk::event_y();
				button = ::fltk::event_button();
				mouse_down(mx,my,button);
				return 1;
			case ::fltk::RELEASE:
				mx = ::fltk::event_x();
				my = ::fltk::event_y();
				button = ::fltk::event_button();
				mouse_up(mx,my,button);
				return 1;
				
			case ::fltk::DRAG:
				mx = ::fltk::event_x();
				my = ::fltk::event_y();
				mouse_move(mx,my);
				return 1;
			case ::fltk::FOCUS:
			case ::fltk::UNFOCUS:
					return 1;
				
					
			case ::fltk::KEY:
				key = ::fltk::event_key();
				key_down(key);
				return 1;

			case ::fltk::KEYUP:
					key = ::fltk::event_key();
					key_up(key);
					return 1;

			case ::fltk::MOUSEWHEEL:
					dy = ::fltk::event_dy();
					mouse_wheel(dy);
					return 1;
				
						
			default:
					return fltk::GlWindow::handle(event);
					
		}			 
	}
			
	


	
public:
  
	///constructor
	gl_view_base(int x,int y,int w,int h): ::fltk::GlWindow(x,y,w,h)
	{
		init_ok=false;
		animation_on=false;
	} 
	///destructor
	virtual ~gl_view_base(){}

	
protected:
	
	
	virtual void reshape(int w, int h){}
	virtual void init(){}
	virtual void render(){}

	virtual void mouse_down(int x, int y,int button){} 
	virtual void mouse_move(int x, int y){}
	virtual void mouse_up(int x, int y,int button) {}
	virtual void key_down(unsigned int key){}
	virtual void key_up( unsigned int key){}
	virtual void mouse_wheel(int dy){}

public:

	///implementation of observer update
	void update()
	{
		update(true);
	}
	
	void update(bool forced)
	{
		if(forced)
		{
			if((init_ok) && this->visible())
				flush();
		}
		else
		{
			redraw();
		}
	}
	void start_animation(int fps=25)
	{
		if(!animation_on)
		{
			::fltk::add_timeout(1.0f/fps,draw_cb,this);
			animation_on=true;
		}
	}
	void stop_animation()
	{
		animation_on=false;
	}

	static inline void draw_cb(void *v)
	{
		if(((gl_view_base*)v)->animation_on)
		{
			((gl_view_base*)v)->update(true);
			::fltk::repeat_timeout(1.0f/((gl_view_base*)v)->fps,draw_cb,v);		
		}	
	}

	
		
	
};

		}
	}
}
	




