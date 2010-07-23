#pragma once

#include <cgv/streamprocessing/streamprocessing.h>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <cgv/streamprocessing/imaging_nodes/raw_image.h>

using namespace cgv::streamprocessing;

namespace cgv {
	namespace streamprocessing {
		namespace imaging_nodes {

template<typename imagetype, int N>
class normalizer_node: public stream_node
{
public:
	port_in<imagetype> input_on;
	port_in<imagetype> input_off;
	port_in<imagetype> input_images[N];

	port_out<imagetype> output_images[N];
	port_out<imagetype> output_shadow_mask;

	typedef typename imagetype::value_type pixel_type;

	



private:
	
	imagetype *on,*off;
	imagetype *onsuboff;
	imagetype *shadow_mask;
	imagetype *workingimages[N];
	pixel_type minonoffdistance;
		

public:

	normalizer_node(pixel_type minonoffdistance=0):stream_node()
	{
		this->minonoffdistance=minonoffdistance;

		register_port_in("on reference image",&input_on);
		register_port_in("off reference image",&input_off);
		register_port_out("shadow mask",&output_shadow_mask);
		
		
		for(int i=0; i < N; i++)
		{
			std::stringstream ss;
			ss << "unnormalized image " << i;
			register_port_in(ss.str(),&input_images[i]);
		}

		
		for(int i=0; i < N; i++)
		{
			std::stringstream ss;
			ss << "normalized image " << i;
			register_port_out(ss.str(),&output_images[i]);
		}

		for(int i=0; i < N; i++)
		{
			workingimages[i] = new imagetype;
		}

		on = new imagetype;
		off = new imagetype;
		onsuboff = new imagetype;
		shadow_mask = new imagetype;
		
		
		
	}

	~normalizer_node()
	{
		delete on;
		delete off;
		delete onsuboff;
		delete shadow_mask;
		for(int i=0; i < N; i++)
		{
			delete workingimages[i];
		}
	}
	
		
	void run()
	{
		std::cout << "start normalizer node\n";			
		while(no_stop_request() )
		{	
			input_on >> &on;	
			input_off >> &off;	
			bool b = (on!=NULL && off!=NULL);
			

			for(int i =0;i < N; i++)
			{
				input_images[i] >> &(workingimages[i]);
				b &= (workingimages[i] != NULL); 
			}

			
			
					
			if(b)
			{
				for(int x=0; x < imagetype.width;x++)
				{
					for(int y=0;y <imagetype.height;y++)
					{
						if((*on)(x,y)>(*off)(x,y))
						{
							(*onsuboff)(x,y) = (*on)(x,y)-(*off)(x,y);
							if((*onsuboff)(x,y) > minonoffdistance)
							{
								(*shadow_mask)(x,y) = 0;
							}
							else
							{
								(*onsuboff)(x,y) = 0;
								(*shadow_mask)(x,y) = 255; 
							}
						}
						else
						{
							(*onsuboff)(x,y) = 0;
							(*shadow_mask)(x,y) = 255; 
							
						}
						for(int i = 0; i < N;i++)
						{
							if((*shadow_mask)(x,y)==0 && (*onsuboff)(x,y) > 0)
							{
								int v = (*(workingimages[i]))(x,y) >(*off)(x,y)?(*(workingimages[i]))(x,y)-(*off)(x,y):0;
								v*=255;
								v/=(*onsuboff)(x,y);
								(*(workingimages[i]))(x,y)=(unsigned char)v;
							}else
							{
								(*(workingimages[i]))(x,y)=0;
							}
						}


					}
				}
				output_shadow_mask << &shadow_mask;
				for(int i = 0; i < N;i++)
				{
					output_images[i] << &(workingimages[i]);
				}

						
				
			}
			
			else
			{
				for(int i =0; i < N; i++)
				{
					imagetype *null = NULL;
					output_images[i] << &null;	
				}
				imagetype *null = NULL;
				output_shadow_mask << &null;

				break;
			}
			
			
		}
		
		std::cout << "stop normalizer node\n";
	}

		
};

		}
	}
}