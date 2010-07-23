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


			///N ... number of graycode bits
	template <typename image_type,int N>
	class graycode_decoder_node : public stream_node
	{
	private:
		image_type *encodedimage;
		image_type *decodedimage;


	public:
		port_in<image_type> input;
		port_out<image_type> output;

		

	
	graycode_decoder_node(int buffersize=1):stream_node(),input(buffersize)
	{
	
		register_port_in("encoded image",&input);	
		register_port_out("decoded image",&output);	
		encodedimage = new image_type;
		decodedimage = new image_type;
		
		
	}

	~graycode_decoder_node()
	{

		delete encodedimage;
		delete decodedimage;

	}
	
		
	void run()
	{
		std::cout << "start graycode decoder node\n";			
		while(no_stop_request() )
		{	
			
			input >> &encodedimage;

			if(encodedimage)
			{
				for(int x=0; x < image_type.width;x++)
				{
					for(int y=0;y <image_type.height;y++)
					{
						(*decodedimage)(x,y) = (*encodedimage)(x,y);
						for(int i = 1; i < N;i++)//bit positions
						{
												
							(*decodedimage)(x,y) = ((*decodedimage)(x,y)) ^ ((*encodedimage)(x,y) >> i);
						
						}
						
							
					}
				}

				output << &decodedimage;			
			}else
			{
				image_type *null = NULL;
				output << &null;
			}
			
			
			
		}
		
		std::cout << "stop graycode decoder node\n";
	}

		
		

		
		
	};
		}
	}
}