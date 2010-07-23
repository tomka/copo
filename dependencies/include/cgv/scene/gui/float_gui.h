#pragma once

#include <fltk/Group.h>
#include <fltk/FloatInput.h>

#include <cgv/utils/observer.h>



namespace cgv{
	namespace scene{
		namespace gui{




class float_gui : public fltk::Group, public cgv::utils::observer
{
	fltk::FloatInput* input;
	cgv::utils::observable< float > *obs;
public:

	float_gui(int x, int y, int w, int h,const char*l, cgv::utils::observable< float> *o) 
		:fltk::Group(x,y,w,h)
	{
		obs = o;
		
		
	
		begin();

		
		int field_width =(int)((0.9f*w*2.0f/3.0f));
		int gap_width =(int)((0.1f*w*2.0f/3.0f));
		int gap_height =(int)(0.1f*h);
		int field_height = (int)(0.9f*h);

		
		input = new fltk::FloatInput((int)(w*1.0/3.0)  ,
		                       gap_height,field_width+gap_width/2,field_height,l);
		
		input->callback(value_changed_cb, this);
		
		end();
		obs->add_observer(this);
		update();
	}

	~float_gui()
	{
		obs->remove_observer(this);
	}

	

	void update()
	{
		
		input->value((*obs->observable_value()));
		
		redraw();
	}

	static inline void value_changed_cb(::fltk::Widget* w, void* v) 
	{
		((float_gui*)v)->commit(); 
	}
	void commit()
	{
		
		(*obs->observable_value()) = (float)input->fvalue();
		
		obs->invalidate();
	}
};
		
		}
	}
}

