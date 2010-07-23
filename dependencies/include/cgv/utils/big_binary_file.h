#pragma once

#include <string>

#ifdef WIN32
#include <windows.h>
#endif


namespace cgv {
	namespace utils {
	/**
	* class to handle files with very large sizes (>=2GB)
	* hiding the ugly win32 api calls
	* 
	* linux-support by using 64bit pointer, LFS required in kernel
	*/
	class big_binary_file
	{
	public:
		enum MODE {READ,WRITE};

	private:
		MODE access_mode;
		std::string filename;
		bool fileopened;

	#ifdef WIN32
		HANDLE hFile; 
	#else
		FILE* file;
	#endif

	public:
		
		///assosiates the new instance to the file filename
		big_binary_file(const std::string &filename);
		
		///close the file (if it is still opened)
		virtual ~big_binary_file();
		
		///open a file in read or write mode
		bool open( MODE m);
		
		///close the file
		void close();
		
		///return true if the file is opened
		bool is_open();

		///return the size of the file in bytes
		long long size();
			
		///read num bytes from the file into the targetbuffer
		bool read(unsigned char *targetbuffer,unsigned long num, unsigned long *numread=NULL);

		///write num bytes to the file from the sourcebuffer (file must be opened with write access first)
		bool write(unsigned char *sourcebuffer,unsigned long num, unsigned long *numwrote=NULL);

		//set the file pointer to index (position in bytes from the beginning of the file)
		bool seek(long long index);
		
		///return the position of the file pointer in bytes 
		long long position();
			
			
		
	};

	}
}

