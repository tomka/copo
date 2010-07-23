#pragma once

#include <cgv/streamprocessing/streamprocessing.h>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <cgv/streamprocessing/imaging_nodes/raw_image.h>




namespace cgv {
	namespace streamprocessing {
		namespace imaging_nodes {

struct vec2f
{
	float x, y;
};

typedef cgv::streamprocessing::imaging_nodes::raw_image<vec2f,1280,1024> flowimage;


class opticalflow_node: public stream_node
{
public:
	port_in<camera_image_type_1280x1024> input;
	port_out<flowimage> output;

	

	



private:
	
	camera_image_type_1280x1024 *image_in;
	camera_image_type_1280x1024 *i1,*i2;
	flowimage *newvel;
	flowimage *oldvel;
	flowimage *vel_out;
	float alpha;
	int iterations;
	
		

public:

	opticalflow_node(float alpha,int iterations) : stream_node()
	{
		
		register_port_in("image in",&input);
		register_port_out("optical flow out",&output);
		
		image_in = new camera_image_type_1280x1024;
		i1 = new camera_image_type_1280x1024;
		i2 = new camera_image_type_1280x1024;
		
		vec2f zero;
		zero.x=0;
		zero.y=0;
		this->alpha = alpha;
		this->iterations = iterations;

		newvel = new flowimage(zero);
		oldvel = new flowimage(zero);
		vel_out = new flowimage();
		
	}

	~opticalflow_node()
	{
		
		delete image_in;
		delete i1;
		delete i2;
		delete newvel;
		delete oldvel;
		delete vel_out;
		
	}



	void calculate_flow(camera_image_type_1280x1024 *i0,camera_image_type_1280x1024 *i1,
		flowimage *oldvel,flowimage *newvel)
	{
		for(int i = 0; i < iterations;i++)
		{
			for(int y = 0; y < 1024;y++)
				for( int x = 0; x < 1280;x++)
			{
					
				float Ex =0.25f * ( (float)(*(i0))(x,y+1,true) - 
									(float)(*(i0))(x,y,true) +
									(float)(*(i0))(x+1,y+1,true) -
									(float)(*(i0))(x+1,y,true) +				
									(float)(*(i1))(x,y+1,true) - 
									(float)(*(i1))(x,y,true) +
									(float)(*(i1))(x+1,y+1,true) -
									(float)(*(i1))(x+1,y,true) );
			
				float Ey =0.25f * ((float)(*(i0))(x+1,y,true) - 
									(float)(*(i0))(x,y,true) +
									(float)(*(i0))(x+1,y+1,true) -
									(float)(*(i0))(x,y+1,true) +				
									(float)(*(i1))(x+1,y,true) - 
									(float)(*(i1))(x,y,true) +
									(float)(*(i1))(x+1,y+1,true) -
									(float)(*(i1))(x,y+1,true));
				
				float Et =0.25f * ((float)(*(i1))(x,y,true) - 
									(float)(*(i0))(x,y,true) +
									(float)(*(i1))(x+1,y,true) -
									(float)(*(i0))(x+1,y,true) +				
									(float)(*(i1))(x,y+1,true) - 
									(float)(*(i0))(x,y+1,true) +
									(float)(*(i1))(x+1,y+1,true) -
									(float)(*(i0))(x+1,y+1,true));
				
				float u_mean = (1.0f/6.0f) *
									( 
									(*oldvel)(x-1,y,true).x  +
									(*oldvel)(x,y+1,true).x  +
									(*oldvel)(x+1,y,true).x  +
									(*oldvel)(x,y-1,true).x  
									)+
							   (1.0f/12.0f) *
									(
									(*oldvel)(x-1,y-1,true).x  +
									(*oldvel)(x-1,y+1,true).x  +
									(*oldvel)(x+1,y+1,true).x  +
									(*oldvel)(x+1,y-1,true).x  
									);
				float v_mean = (1.0f/6.0f) *
									( 
									(*oldvel)(x-1,y,true).y  +
									(*oldvel)(x,y+1,true).y  +
									(*oldvel)(x+1,y,true).y  +
									(*oldvel)(x,y-1,true).y  
									)+
							   (1.0f/12.0f) *
									(
									(*oldvel)(x-1,y-1,true).y  +
									(*oldvel)(x-1,y+1,true).y  +
									(*oldvel)(x+1,y+1,true).y  +
									(*oldvel)(x+1,y-1,true).y  
									);

					(*newvel)(x,y).x=u_mean - Ex*(Ex*u_mean +Ey*v_mean +Et)/(alpha*alpha + Ex*Ex +Ey*Ey);
					(*newvel)(x,y).y=v_mean - Ey*(Ex*u_mean +Ey*v_mean +Et)/(alpha*alpha + Ex*Ex +Ey*Ey);
				}
			

			(*oldvel)=(*newvel);
		}

	}
				
	
		
	void run()
	{
		std::cout << "start optical flow node\n";			
		input >> &image_in;
		*i1 =*image_in;
		int counter=0;
		while(no_stop_request() )
		{
			input >> &image_in;
			if(image_in)
			{
				*i2 = *image_in;
			
				calculate_flow(i1,i2,oldvel,newvel);
				*i1 = *i2;
			
				*vel_out = *newvel;
				*oldvel = *newvel;
				output << &vel_out;	
				counter++;
				std::cout << counter <<" flow frames calculated\n";
				
			}else
			{
				flowimage *null = NULL;
				output << &null;
				break;
			}

					
			
		}
		
		std::cout << "stop optical flow node\n";
	}

		
};

		}
	}
}