#pragma once
#include <cgv/streamprocessing/streamprocessing.h>
#include <sstream>

namespace cgv {
	namespace streamprocessing {
		namespace basic_nodes {

template<typename input_t, int N>
class copy_node: public stream_node
{
public:
	port_in<input_t> input;
	port_out<input_t> output[N];


private:
	
	input_t *data[2];	

public:

	copy_node( int buffersize=1):stream_node(),input(buffersize)
	{
		register_port_in("input data",&input);
		for(int i = 0; i < N; i++)
		{
			std::stringstream ss;
			ss << "output data " << i;

			register_port_out(ss.str(),&(output[i]));
		}
		data[0] = new input_t;
		data[1] = new input_t;
	
	}

	~copy_node()
	{
		delete data[0];
		delete data[1];
		
	}
	
		
	void run()
	{
		std::cout << "start copy node\n";
		int i =0;
		
		while(no_stop_request() )
		{	
			input >> &(data[0]);	
			
			
					
			if(data[0])
			{
				
				for(int i = 0; i < N;i++)
				{
					*(data[1])=*(data[0]);
					output[i] << &(data[1]);	
				}
			}
			
			else
			{
				input_t* null;
				
				for(int j = 0; j < N;j++) 
				{
					null=NULL;
					output[j] << &null;	
				}
				break;
			}
			
			
		}
		
		std::cout << "stop copy node\n";
	}

		
};
		}
	}
}