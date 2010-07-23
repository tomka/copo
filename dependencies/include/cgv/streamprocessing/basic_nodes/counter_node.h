#pragma once
#include <cgv/streamprocessing/streamprocessing.h>


using namespace cgv::streamprocessing;

namespace cgv {
	namespace streamprocessing {
		namespace basic_nodes {


template<typename input_t>
class counter_node: public stream_node
{
public:
	port_in<input_t> input;
	port_out<input_t> output;

		


private:
	
	input_t *data;	
	unsigned long count;
	

public:

	counter_node():stream_node(),input(1)
	{
		register_port_in("input data",&input);
		register_port_out("output data",&output);
		data = new input_t;
		count = 0;
	
	}

	~counter_node()
	{
		delete data;
		
	}

	unsigned long count_value()
	{	
		return count;	
	}

	
		
	void run()
	{
		count = 0;
		std::cout << "start counter node\n";
		
		while(no_stop_request() )
		{	
			input >> &data;	
			
			if(data)
			{
				output << &data;
				count++;
				
			}else
			{
				output << &data;
				break;
			}

		
		}
		
		std::cout << "stop counter node\n";
	}

		
};

		}
	}
}