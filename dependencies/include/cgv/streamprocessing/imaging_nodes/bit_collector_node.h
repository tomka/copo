#pragma once
#include <cgv/streamprocessing/streamprocessing.h>
#include <sstream>


using namespace cgv::streamprocessing;


namespace cgv {
	namespace streamprocessing {
		namespace imaging_nodes {

///N number of bits to collect
template<typename image_type_in,typename image_type_out, int N>
class bit_collector_node: public stream_node
{
public:
	port_in<image_type_in> input;
	port_out<image_type_out> output;


private:
	
	image_type_in *data_in;
	image_type_out *data_out;


public:

	bit_collector_node():stream_node()
	{
		
		register_port_in("single bit",&input);
		register_port_out("shifted bits",&output);
			
		
		data_in = new image_type_in;
		data_out = new image_type_out;
		
	
	}

	~bit_collector_node()
	{
		
		delete data_in;
		delete data_out;
		
	}
	
		
	void run()
	{
		std::cout << "start bit collector node\n";
	
		int i=0;
		while(no_stop_request() )
		{
			
			for(int i = 0; i < N; i++)
			{	
				input >> &data_in;						
				if(data_in)
				{
					if(i ==0)
					{
						for(int x=0; x < image_type_out.width;x++)
						{
							for(int y=0;y <image_type_out.height;y++)
							{
								(*data_out)(x,y) = (*data_in)(x,y);
							}
						}
					}else
					{
						for(int x=0; x < image_type_out.width;x++)
						{
							for(int y=0;y <image_type_out.height;y++)
							{					
								(*data_out)(x,y) += ((*data_in)(x,y) > 0) ? (1<<i): 0;
							}
						}
					}
					
				}		
				else
				{							
					image_type_out* null;
					null=NULL;
					output << &null;
					std::cout << "stop bit collector node\n";
					return;
				}
			}
			output << &data_out;
				
		}
		std::cout << "stop bit collector node\n";
			
			
	
		
		
	}

		
};

		}
	}
}



