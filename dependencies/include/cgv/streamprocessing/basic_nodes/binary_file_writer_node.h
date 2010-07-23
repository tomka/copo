#pragma once

#include <cgv/streamprocessing/streamprocessing.h>
#include <cgv/utils/big_binary_file.h>

using namespace cgv::streamprocessing;



namespace cgv {
	namespace streamprocessing {
		namespace basic_nodes {

template <typename T>
class binary_file_writer_node: public stream_node
{
public:
	port_in<T> input;


private:
	cgv::utils::big_binary_file *file;
	unsigned long block_size;
	T *block;
	long counter ;

public:

	binary_file_writer_node(const std::string filename,int buffersize=1):stream_node(),input(buffersize)
	{
		register_port_in("binary block",&input);

		file = new cgv::utils::big_binary_file(filename);	
		block_size = sizeof(T);
		
		block = new T();
		counter=0;
	}
		
	~ binary_file_writer_node()
	{
		delete file;		
		delete block;
		
	}

	
	void run()
	{
		std::cout << "start binary writer node\n";
		file->open(cgv::utils::big_binary_file::WRITE);		
		while(no_stop_request() )
		{
			
			input >> &block;
			
	
				
						
			if(block)
			{
				counter++;
				unsigned long written;
				file->write((unsigned char *)(block),block_size,&written);
				//std::cout << "written\n";
				
				
			}
			
			else
				break;
			
		}
		file->close();	
		std::cout << "stop binary writer node\n";
	}

		
};

		}
	}
}