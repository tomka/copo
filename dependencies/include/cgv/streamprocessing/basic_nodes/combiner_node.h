#pragma once
#include <cgv/streamprocessing/streamprocessing.h>
#include <sstream>


using namespace cgv::streamprocessing;


namespace cgv {
	namespace streamprocessing {
		namespace basic_nodes {

// a node with N input and one output ports. 
// The opposite node of a splitter. 
template<typename input_t, int N>
class combiner_node: public stream_node
{
public:
	port_in<input_t> input[N];
	port_out<input_t> output;


private:
	
	input_t *data;	

public:

	combiner_node():stream_node()
	{
		register_port_out("output data",&output);
		for(int i = 0; i < N; i++)
		{
			std::stringstream ss;
			ss << "input data" << i;

			register_port_in(ss.str(),&(input[i]));
			
		}
		data = new input_t;
		
	
	}

	~combiner_node()
	{
		
		delete data;
		
	}
	
		
	void run()
	{
		std::cout << "start combiner node\n";
		int i =0;
		while(no_stop_request() )
		{	
			input[i] >> &data;	
			i=(i+1)	%N;
						
					
			if(data)
			{
				output << &data;	
				
			}
			
			else
			{
				
			
				if(i == N-1)
				{
									
					input_t* null;
					{
						null=NULL;
						output << &null;	
					}
					break;
				}
				
			}
			
			
		}
		
		std::cout << "stop combiner node\n";
	}

		
};

		}
	}
}



