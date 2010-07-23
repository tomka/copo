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
class normal_correlation_node: public stream_node
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

	normal_correlation_node(const masktype &m,bool absolut=false,int buffersize=1):stream_node(),input(buffersize)
	{
		register_port_in("input image",&input);
		register_port_out("correlated image",&output);
		workingimage = new imagetype;
		workingimage2 = new imagetype;
		mask = m;
		this->absolut=absolut;
	}

	~normal_correlation_node()
	{
		delete workingimage;
		delete workingimage2;
	}
	
		
	void run()
	{
		std::cout << "start normal correlation node\n";			
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
							float f_mean = 0;
							float g_mean = 0;

							for(int i = 0; i < mask.width;i++)
							{
								mx=x-mask.center_x;
								for(int j = 0; j < mask.height;j++)
								{
									f_mean += ((float)mask(i,j));
									g_mean += (*workingimage)(mx,my,true);
									mx++;				
								}
								my++;
								
								
							}
							f_mean/=(mask.width*mask.height);
							g_mean/=(mask.width*mask.height);

							float sigma_f=0,sigma_g=0,sigma_fg=0;
							for(int i = 0; i < mask.width;i++)
							{
								mx=x-mask.center_x;
								for(int j = 0; j < mask.height;j++)
								{
									sigma_fg += (((float)mask(i,j))-f_mean)*((*workingimage)(mx,my,true)-g_mean );
									sigma_f += (((float)mask(i,j))-f_mean)*(((float)(mask)(i,j))-f_mean );
									sigma_g += ((*workingimage)(mx,my,true)-g_mean )*((*workingimage)(mx,my,true)-g_mean );

									mx++;				
								}
								my++;
								
								
							}
							sigma_fg /=(mask.width*mask.height);
							sigma_f/=(mask.width*mask.height);
							sigma_g/=(mask.width*mask.height);
							sigma_f=sqrt(sigma_f);
							sigma_g=sqrt(sigma_g);

														
							
							float rho =(unsigned char)(255*(sigma_fg/(sigma_f*sigma_g)));
							(*workingimage2)(x,y)=rho>200?rho:0;
						


					}
				

				output <<&workingimage2;	
			}
			
			else
			{
				output <<&workingimage;	
				break;
			}
			
			
		}
		
		std::cout << "stop normal correlation node\n";
	}

		
};

		}
	}
}