#pragma once

#include <fltk/Group.h>
#include <fltk/FloatInput.h>
#include <cgv/utils/observer.h>
#include <cgv/scene/geometry/sphere.h>



namespace cgv{
	namespace scene{
		namespace gui{




template<int N>
class sphere_gui : public fltk::Group, public cgv::utils::observer
{
	fltk::FloatInput* inputs[N+1];
	cgv::utils::observable< cgv::scene::geometry::sphere<N,float> > *obs;

	
	
	
public:

	sphere_gui(int x, int y, int w, int h,const char*l, cgv::utils::observable< cgv::scene::geometry::sphere<N,float> > *o) 
		:fltk::Group(x,y,w,h)
	{
		obs = o;
			
	
		begin();

		int field_width =(int)((0.9f*w*2.0f/3.0f)/N);
		int field_width2 =(int)((0.9f*w*2.0f/3.0f));
		int gap_width =(int)((0.1f*w*2.0f/3.0f)/N);
		int gap_width2 =(int)((0.1f*w*2.0f/3.0f));
		int gap_height =(int)(0.1f*h/2);
		int field_height = (int)(0.9f*h/2);

		for(int i = 0; i < N;i++)
		{
			

			if(i==0)			
			{
				inputs[i] = new fltk::FloatInput((int)(w*1.0/3.0) + i*(field_width+gap_width),
				                       gap_height,field_width,field_height,"position");
			}
			else
			{
				inputs[i] = new fltk::FloatInput((int)(w*1.0/3.0) + i*(field_width+gap_width),
				                       gap_height,field_width,field_height);
			}
			inputs[i]->callback(value_changed_cb, this);
		}
		inputs[N] = new fltk::FloatInput((int)(w*1.0/3.0) ,  2*gap_height+field_height,
									field_width2+gap_width2/2,field_height,"radius");
		inputs[N]->callback(value_changed_cb, this);
		end();
		obs->add_observer(this);
		update();
	}

	~sphere_gui()
	{
		obs->remove_observer(this);
	}

		
	

	void update()
	{
		for(int i = 0; i < N;i++)
		{
			inputs[i]->value(obs->observable_value()->center(i));
		}
		inputs[N]->value(obs->observable_value()->radius);

		redraw();		
		
	}

	static inline void value_changed_cb(::fltk::Widget* w, void* v) 
	{
		((sphere_gui*)v)->commit(); 
	}

	void commit()
	{
		for(int i = 0; i < N;i++)
		{
			obs->observable_value()->center(i) = (float)inputs[i]->fvalue();
		}
		obs->observable_value()->radius=(float)(inputs[N]->fvalue());
		obs->invalidate();
	}
};
		
		}
	}
}

