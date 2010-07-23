#pragma once
#include <cgv/streamprocessing/imaging_nodes/raw_image.h>
#include <cgv/streamprocessing/streamprocessing.h>
#include <sstream>
#include <algorithm>
#include <math.h>


using namespace cgv::streamprocessing;

namespace cgv {
	namespace streamprocessing {
		namespace imaging_nodes {

template<typename imagetype>
class threshold_node: public stream_node
{
public:
	port_in<imagetype> input;
	port_out<imagetype> output;
	
	typedef typename imagetype::value_type pixel_type;


private:
	bool no_max;
	imagetype *workingimage;
	pixel_type min, max,threshold;
	

public:

	threshold_node(pixel_type min, pixel_type max, pixel_type threshold,
		int buffersize=1):stream_node(),input(buffersize)
	{
		no_max = false;
		register_port_in("image",&input);
		register_port_out(" threshold image",&output);	
		workingimage = new imagetype;
		this->min = min;
		this->max = max;
		this->threshold = threshold;
		
	}

	threshold_node(pixel_type min, pixel_type threshold,
		int buffersize=1):stream_node(),input(buffersize)
	{
		register_port_in("image",&input);
		register_port_out(" threshold image",&output);	
		workingimage = new imagetype;
		this->min = min;
		this->threshold = threshold;
		no_max = true;
		
	}

	~threshold_node()
	{

		delete workingimage;
	}
	
		
	void run()
	{
		std::cout << "start threshold node\n";			
		while(no_stop_request() )
		{	
			input >> &workingimage;	
			
			
					
			if(workingimage)
			{
				if(no_max)
				{
					for(int x=0; x < imagetype.width;x++)
						for(int y=0;y <imagetype.height;y++)
						{			
							(*workingimage)(x,y)=(*workingimage)(x,y)>threshold? (*workingimage)(x,y):min;
						}
				}else
				{
					for(int x=0; x < imagetype.width;x++)
						for(int y=0;y <imagetype.height;y++)
						{
							(*workingimage)(x,y)=(*workingimage)(x,y)>threshold? max:min;
						}

				}
					

				output <<&workingimage;	
			}
			
			else
			{
				output <<&workingimage;	
				break;
			}
			
			
		}
		
		std::cout << "stop threshold node\n";
	}

		
};



		}
	}
}