#pragma once
#include <cgv/streamprocessing/streamprocessing.h>
#include <sstream>


using namespace cgv::streamprocessing;


namespace cgv {
	namespace streamprocessing {
		namespace basic_nodes {

// a node with N inputs and one output ports. 

template<typename input_t, int N>
class collector_node: public stream_node
{
public:
	port_in<input_t> input[N];
	port_out<input_t> output;


private:
	
	input_t *data;	

public:

	collector_node():stream_node()
	{
		register_port_out("output data",&output);
		for(int i = 0; i < N; i++)
		{
			std::stringstream ss;
			ss << "input data" << i;

			register_port_in(ss.str(),&(input[i]));
			
		}
		
		
	
	}

	~collector_node()
	{
		
		delete data;
		
	}
	
		
	void run()
	{
		std::cout << "start collector node\n";
		for(int i = 0;i < N && no_stop_request();i++)
		{
			data = new input_t;
			
			while(no_stop_request())
			{
				input[i] >> &data;	
				if(data)
					output << &data;
				else
					break;
			}
			

		}
		input_t *null=NULL;
		output << &null;	
		std::cout << "stop collector node\n";
	}

		
};



		}
	}
}