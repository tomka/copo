#pragma once

#include <fltk/Group.h>
#include <fltk/Input.h>

#include <cgv/utils/observer.h>
#include <string>



namespace cgv{
	namespace scene{
		namespace gui{





class string_gui : public fltk::Group, public cgv::utils::observer
{
	fltk::Input* input;
	cgv::utils::observable< std::string > *obs;
public:

	string_gui(int x, int y, int w, int h,const char*l, cgv::utils::observable< std::string > *o) 
		:fltk::Group(x,y,w,h)
	{
		obs = o;
		
		
	
		begin();

		
		int field_width =(int)((0.9f*w*2.0f/3.0f));
		int gap_width =(int)((0.1f*w*2.0f/3.0f));
		int gap_height =(int)(0.1f*h);
		int field_height = (int)(0.9f*h);

		
		input = new fltk::Input((int)(w*1.0/3.0) ,
		                       gap_height,field_width+gap_width/2,field_height,l);
		
		input->when(fltk::WHEN_ENTER_KEY );
		input->callback(value_changed_cb, this);
		
		end();
		obs->add_observer(this);
		update();
	}

	~string_gui()
	{
		obs->remove_observer(this);
	}

	


	void update()
	{
		
		input->text((*obs->observable_value()).c_str());
		
		redraw();
	}

	static inline void value_changed_cb(::fltk::Widget* w, void* v) 
	{
		((string_gui*)v)->commit(); 
	}
	void commit()
	{
		
		(*obs->observable_value()) = input->text();
		
		obs->invalidate();
	}
};
		
		}
	}
}

