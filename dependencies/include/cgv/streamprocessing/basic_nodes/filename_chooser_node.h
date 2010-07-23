#pragma once

#include <cgv/streamprocessing/streamprocessing.h>
#include <cgv/utils/big_binary_file.h>
#include <fltk/FileChooser.h>
#include <fltk/filename.h>
#include <fltk/run.h>

using namespace cgv::streamprocessing;

namespace cgv {
	namespace streamprocessing {
		namespace basic_nodes {


class filename_chooser_node : public stream_node
{
public:
	port_out<std::string> output;


private:
	
	
	std::vector< std::string* > filenames;
	std::string path;
	std::string pattern;
	std::string title;


public:

	filename_chooser_node(const std::string title,const std::string pattern="*.*",
		const std::string path=""):stream_node()
	{
		this->title = title;
		this->pattern = pattern;
		this->path = path;
		register_port_out("filename",&output);
		
		::fltk::FileChooser fc(path.c_str(),pattern.c_str(),::fltk::FileChooser::MULTI,title.c_str());
		fc.preview(0);
		
		
		fc.show();
			
		
		while(fc.visible()){::fltk::wait();}
		
		
		for(int i = 1; i <= fc.count(); i++)
		{
			filenames.push_back(new std::string(fc.value(i)));
			
			
		}
		
	
		
	
	}
		
	~filename_chooser_node()
	{
		for(std::vector< std::string* >::iterator it = filenames.begin();it != filenames.end();it++)
		{	
			delete *it;
		}
		
		
	}

	
	void run()
	{
		std::cout << "start file chooser \n";
		
		

	
		
		
		for(std::vector< std::string* >::iterator it = filenames.begin();it != filenames.end();it++)
		{	
			std::cout <<"Chosen file:" << **it<<std::endl;
			output << &(*it);
			
		}
	
		std::string* null = NULL;
		output << &null;
		std::cout << "stop file chooser \n";
	}

		
};


		}
	}
}