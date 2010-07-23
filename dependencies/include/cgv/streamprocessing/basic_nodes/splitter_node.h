#pragma once
#include <cgv/streamprocessing/streamprocessing.h>
#include <sstream>


using namespace cgv::streamprocessing;

// a node with one input and N output ports. The ith input goes 
// directly to the (i modulo N) output.

namespace cgv {
	namespace streamprocessing {
		namespace basic_nodes {

template<typename input_t, int N>
class splitter_node: public stream_node
{
public:
	port_in<input_t> input;
	port_out<input_t> output[N];


private:
	
	input_t *data;	

public:

	splitter_node( int buffersize=1):stream_node(),input(buffersize)
	{
		register_port_in("input data",&input);
		for(int i = 0; i < N; i++)
		{
			std::stringstream ss;
			ss << "output data " << i;

			register_port_out(ss.str(),&(output[i]));
		}
		data = new input_t;
	
	}

	~splitter_node()
	{
		delete data;
		
	}
	
		
	void run()
	{
		std::cout << "start splitter\n";
		int i =0;
		while(no_stop_request() )
		{	
			input >> &data;	
			
			
					
			if(data)
			{
				output[i] <<&data;	
				i=(i+1)	%N;
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
		
		std::cout << "stop splitter\n";
	}

		
};
		}
	}
}





