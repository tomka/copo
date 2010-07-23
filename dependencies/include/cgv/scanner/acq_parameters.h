#pragma once
#include <iostream>
#include <string>
#include<fstream>

namespace cgv{
	namespace scanner{
	

struct acq_parameters
{
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
	unsigned int expt;
	double framerate;
	double gain;
	unsigned char ds;

	bool write_to_file(const std::string filename)
	{
		std::cout << "writing camera parameters into "<<filename<<std::endl;
		
		std::fstream file;
		file.open(filename.c_str(),std::ios_base::out);
		if(!file)
			return false;
			
		file << x << std::endl;
		file << y << std::endl;
		file << width << std::endl;
		file << height << std::endl;
		file << expt<< std::endl;
		file << framerate << std::endl;
		file << gain << std::endl;
		file << ds << std::endl;
		file.close();
		return true;
	}

	bool read_from_file(const std::string filename)
	{
		std::cout << "reading camera parameters from " << filename<<std::endl;
		std::fstream file;
		file.open(filename.c_str(),std::ios_base::in);
		if(!file)
			return false;
		
		file >> x; 
		file >> y; 
		file >> width;
		file >> height;
		file >> expt;
		file >> framerate;
		file >> gain;
		file >> ds;

		file.close();
		return true;
	}
};
	}
}

