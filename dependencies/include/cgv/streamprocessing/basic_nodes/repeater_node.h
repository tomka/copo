#pragma once
#include <cgv/streamprocessing/streamprocessing.h>
#include <sstream>

namespace cgv {
	namespace streamprocessing {
		namespace basic_nodes {

template<typename input_t, int N>
class repeater_node: public stream_node
{
public:
	port_in<input_t> input;
	port_out<input_t> output;


private:
	
	input_t *data[2];	

public:

	repeater_node( int buffersize=1):stream_node(),input(buffersize)
	{
		register_port_in("input data",&input);
		for(int i = 0; i < N; i++)
		{
			
			register_port_out("repeated output",&(output));
		}
		data[0] = new input_t;
		data[1] = new input_t;
	
	}

	~repeater_node()
	{
		delete data[0];
		delete data[1];
		
	}
	
		
	void run()
	{
		std::cout << "start repeater node\n";
		int i =0;
		
		while(no_stop_request() )
		{	
			input >> &(data[0]);	
			
			
					
			if(data[0])
			{
				
				for(int i = 0; i < N;i++)
				{
					*(data[1])=*(data[0]);
					output << &(data[1]);	
				}
			}
			
			else
			{
				input_t* null;
								
				null=NULL;
				output << &null;	
				
				break;
			}
			
			
		}
		
		std::cout << "stop repeater node\n";
	}

		
};
		}
	}
}