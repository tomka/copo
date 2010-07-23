#pragma once

#include <fltk/Group.h>
#include <fltk/FloatInput.h>

#include <cgv/utils/observer.h>
#include <cgv/scene/geometry/direction.h>


namespace cgv{
	namespace scene{
		namespace gui{




template<int N>
class direction_gui : public fltk::Group, public cgv::utils::observer
{
	fltk::FloatInput* inputs[N];
	cgv::utils::observable< cgv::scene::geometry::direction<float,N> > *obs;
public:

	direction_gui(int x, int y, int w, int h,const char*l, cgv::utils::observable< cgv::scene::geometry::direction<float,N> > *o) 
		:fltk::Group(x,y,w,h)
	{
		obs = o;
		
		
	
		begin();

		for(int i = 0; i < N;i++)
		{
			int field_width =(int)((0.9f*w*2.0f/3.0f)/N);
			int gap_width =(int)((0.1f*w*2.0f/3.0f)/N);
			int gap_height =(int)(0.1f*h);
			int field_height = (int)(0.9f*h);

			if(i==0)			
			{
				inputs[i] = new fltk::FloatInput((int)(w*1.0/3.0) + i*(field_width+gap_width),
				                       gap_height,field_width,field_height,l);
			}
			else
			{
				inputs[i] = new fltk::FloatInput((int)(w*1.0/3.0) + i*(field_width+gap_width),
				                       gap_height,field_width,field_height);
			}
			inputs[i]->callback(value_changed_cb, this);
		}
		end();
		obs->add_observer(this);
		update();
	}

	~direction_gui()
	{
		obs->remove_observer(this);
	}

	

	
	

	void update()
	{
		for(int i = 0; i < N;i++)
		{
			inputs[i]->value((*obs->observable_value())(i));
		}
		redraw();
	}

	static inline void value_changed_cb(::fltk::Widget* w, void* v) 
	{
		((direction_gui*)v)->commit(); 
	}
	void commit()
	{
		for(int i = 0; i < N;i++)
		{
			(*obs->observable_value())(i) = (float)inputs[i]->fvalue();
		}
		obs->invalidate();
	}
};
		
		}
	}
}

