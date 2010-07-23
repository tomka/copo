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

template<typename imagetype,int N>
class morphologic_filter_node: public stream_node
{
public:
	port_in<imagetype> input;
	port_out<imagetype> output;

	typedef typename imagetype::value_type pixel_type;

	enum Type{MINIMUM_FILTER=0,MEDIAN_FILTER=N,MAXIMUM_FILTER=2*N};
	int pos;
	
private:
	std::vector<pixel_type> window;
	imagetype *workingimage;
	imagetype *workingimage2;
public:

	morphologic_filter_node(Type filtertype,int buffersize=1):stream_node(),input(buffersize)
	{
		register_port_in("unfiltered image",&input);
		register_port_out("filtered image",&output);
		workingimage = new camera_image_type;
		workingimage2 = new camera_image_type;
		pos = (int)filtertype;

	}

	~morphologic_filter_node()
	{
		delete workingimage;
		delete workingimage2;
	}

	void run()
	{
		std::cout << "start morphologic filter node\n";			
		while(no_stop_request() )
		{	
			input >> &workingimage;	
			
					
			if(workingimage)
			{
				for(int x=0; x < imagetype.width;x++)
					for(int y=0;y <imagetype.height;y++)
					{
						
							int mx=x-N;
							int my=y-N;
							
							for(int i = 0; i < 2*N+1;i++)
							{
								mx=x-N;
								for(int j = 0; j < 2*N+1;j++)
								{
									window.push_back((*workingimage)(mx,my,true));
									mx++;				
								}
								my++;
								
								
							}

							std::sort(window.begin(),window.end());
							(*workingimage2)(x,y)=window[pos];
							window.clear();
						


					}
				//filter code
					
				output <<&workingimage2;	
			}
			
			else
			{
				output <<&workingimage;	
				break;
			}
			
			
		}
		
		std::cout << "stop morphologic filter node\n";
	}
	

};

		}
	}
}