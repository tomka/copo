#pragma once
#include <cgv/streamprocessing/streamprocessing.h>


using namespace cgv::streamprocessing;

namespace cgv {
	namespace streamprocessing {
		namespace basic_nodes {


template<typename input_t>
class null_node: public stream_node
{
public:
	port_in<input_t> input;
	


private:
	
	input_t *data;	

public:

	null_node(int buffersize=10):stream_node(),input(buffersize)
	{
		register_port_in("input data",&input);
		data = new input_t;
	
	}

	~null_node()
	{
		delete data;
		
	}
	
		
	void run()
	{
		std::cout << "start null node\n";
		while(no_stop_request() )
		{	
			input >> &data;	
			
			
			if(!data)
				break;
		
		}
		
		std::cout << "stop null node\n";
	}

		
};

		}
	}
}