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

template<typename imagetype,typename masktype>
class linear_filter_node: public stream_node
{
public:
	port_in<imagetype> input;
	port_out<imagetype> output;


private:
	
	imagetype *workingimage;
	imagetype *workingimage2;
	masktype mask;
	bool absolut;
	

public:

	linear_filter_node(const masktype &m,bool absolut=false,int buffersize=1):stream_node(),input(buffersize)
	{
		register_port_in("unfiltered image",&input);
		register_port_out("filtered image",&output);
		workingimage = new imagetype;
		workingimage2 = new imagetype;
		mask = m;
		this->absolut=absolut;
	}

	~linear_filter_node()
	{
		delete workingimage;
		delete workingimage2;
	}
	
		
	void run()
	{
		std::cout << "start linear filter node\n";			
		while(no_stop_request() )
		{	
			input >> &workingimage;	
			
					
			if(workingimage)
			{
				for(int x=0; x < imagetype.width;x++)
					for(int y=0;y <imagetype.height;y++)
					{
						
							int mx=x-mask.center_x;
							int my=y-mask.center_y;
							float v = 0;

							for(int i = 0; i < mask.width;i++)
							{
								mx=x-mask.center_x;
								for(int j = 0; j < mask.height;j++)
								{
									v += ((float)mask(i,j))*(*workingimage)(mx,my,true);
									mx++;				
								}
								my++;
								
								
							}

							if(absolut)
								v=fabs(v);

							int av = (int)(v+0.5f);
							av = av > 255? 255:av;
							av = av < 0? 0:av;
							//std::cout <<"(" <<av<<","<<v <<")";
							
							(*workingimage2)(x,y)=(unsigned char)av;
						


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
		
		std::cout << "stop linear filter node\n";
	}

		
};

		}
	}
}