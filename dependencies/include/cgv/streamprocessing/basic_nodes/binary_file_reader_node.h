#pragma once
#include <cgv/streamprocessing/streamprocessing.h>
#include <cgv/utils/big_binary_file.h>

using namespace cgv::streamprocessing;

namespace cgv {
	namespace streamprocessing {
		namespace basic_nodes {

template <typename T>
class binary_file_reader_node : public stream_node
{
public:
	port_out<T> output;
	port_in<std::string> input;


private:
	cgv::utils::big_binary_file *file;
	unsigned long blocks_to_read;
	unsigned long block_size;
	T *block;
	std::string *filename;

public:

	binary_file_reader_node():stream_node()
	{
		register_port_out("binary block",&output);
		register_port_in("filename",&input);
		filename = new std::string;
		file=NULL;
		block=NULL;		
	}

		
	~ binary_file_reader_node()
	{
		if(file)
			delete file;
		if(block)
			delete block;
		delete filename;
		
	}

	
	void run()
	{
		std::cout << "start binary reader node \n";
		while(no_stop_request())
		{
			input >> &filename;
			if(filename)
			{
				file = new cgv::utils::big_binary_file(*filename);	
				block_size = sizeof(T);
			
				blocks_to_read = (unsigned long)(file->size()/block_size);	
				block = new T();
				file->open(cgv::utils::big_binary_file::READ);	
				
				while( blocks_to_read >0 )
				{
					file->read((unsigned char *)(block),block_size);
					blocks_to_read--;
					output << &block;
					

				}
				file->close();	
			}else
			{
				break;
			}
		}
		T* null = NULL;
		output << &null;
		std::cout << "stop binary reader node\n";
	}

		
};

template <typename T>
class single_binary_file_reader_node : public stream_node
{
public:
	port_out<T> output;
	


private:
	cgv::utils::big_binary_file *file;
	unsigned long blocks_to_read;
	unsigned long block_size;
	T *block;
	std::string *filename;

public:

	single_binary_file_reader_node(const std::string filename):stream_node()
	{
		register_port_out("binary block",&output);
		
		this->filename = new std::string(filename);
		file=NULL;
		block=NULL;		
	}

		
	~single_binary_file_reader_node()
	{
		if(file)
			delete file;
		if(block)
			delete block;
		delete filename;
		
	}

	
	void run()
	{
		std::cout << "start single binary reader node \n";
		
			
		if(filename)
		{
			file = new cgv::utils::big_binary_file(*filename);	
			block_size = sizeof(T);
		
			blocks_to_read = (unsigned long)(file->size()/block_size);	
			block = new T();
			file->open(cgv::utils::big_binary_file::READ);	
				
			while( blocks_to_read >0 )
			{
				file->read((unsigned char *)(block),block_size);
				blocks_to_read--;
				output << &block;
					

			}
			file->close();	
		}
		
		T* null = NULL;
		output << &null;
		std::cout << "stop single binary reader node\n";
	}

		
};


		}
	}
}