#include <cgv/utils/big_binary_file.h>
#include <iostream>

namespace cgv {
	namespace utils{

	#ifdef WIN32

	big_binary_file::big_binary_file(const std::string &filename)
	{
		this->filename = filename;
		fileopened=false;
	};
		
	bool big_binary_file::open( MODE m)
	{	
			if(fileopened)
				close();

			WCHAR name[256];
			MultiByteToWideChar(CP_ACP, 0, filename.c_str(),(int)filename.size() +1, name,
					sizeof(name)/sizeof(name[0]) );

			access_mode = m;
			if(m == READ)
			{
				//open for read
				hFile = CreateFile(name,						
						GENERIC_READ,					
							FILE_SHARE_READ,            
							NULL,                       
							OPEN_EXISTING,               
							FILE_ATTRIBUTE_NORMAL,      
							NULL);

			}else
			{
				//open for write
				hFile = CreateFile(name,           
						 GENERIC_WRITE,               
						0,                           
						NULL,                        
						CREATE_ALWAYS,               
						FILE_ATTRIBUTE_NORMAL,      
						NULL);  
			}
			fileopened=true;
			return true;
	}


	void big_binary_file::close()
	{
		if(fileopened)
			CloseHandle(hFile);	
		fileopened=false;

	}

	bool big_binary_file::is_open()
	{
		return fileopened;
	}


	long long big_binary_file::size()
	{
			if(fileopened)
			{
				LARGE_INTEGER fs;
				
				if(GetFileSizeEx(hFile,(PLARGE_INTEGER) &fs))
					return fs.QuadPart;
				else
					return -1;
			}
			else
			{
				open(READ);

				LARGE_INTEGER fs;
				long long value;
				
				if(GetFileSizeEx(hFile,(PLARGE_INTEGER) &fs))
					value = fs.QuadPart;
				else
					value = -1;

				close();
				return value;
			}
		}
		
	bool big_binary_file::read(unsigned char *targetbuffer,unsigned long num, unsigned long *numread)
		{
			if(fileopened && access_mode == READ)
			{
				if(numread == NULL)
				{
					unsigned long _numread;
					ReadFile(hFile,targetbuffer,num,&_numread,NULL);
					return num == _numread;
				}
				else
				{
					ReadFile(hFile,targetbuffer,num,numread,NULL);
					return  num == *numread;
				} 
			}
			return false;
		}

	bool big_binary_file::write(unsigned char *sourcebuffer,unsigned long num, unsigned long *numwrote)
		{
			if(fileopened && access_mode == WRITE)
			{
				if(numwrote == NULL)
				{
					unsigned long _numwrote;
					WriteFile(hFile,sourcebuffer, num, &_numwrote, NULL);
					return _numwrote == num;
				}
				else
				{
					if(!WriteFile(hFile,sourcebuffer, num, numwrote, NULL))

					{
						std::cerr << "write error"<<std::endl;		
						
					}

					return *numwrote == num;
				}
			}
			return false;
		}
		
	bool big_binary_file::seek(long long index)
		{
			if(fileopened)
			{
				LARGE_INTEGER l;
					l.QuadPart=(index);
			
				if(SetFilePointerEx(hFile,l,NULL,FILE_BEGIN))
					return true;
				else
					return false;
			}
			return false;
		}

	long long big_binary_file::position()
		{
			if(fileopened)
			{
				LARGE_INTEGER l1,l2;
				l1.QuadPart=0;
				if(SetFilePointerEx(hFile,l1,&l2,FILE_CURRENT))
					return l2.QuadPart;
				else
					return -1;

			}
			else
				return false;

		}
		

	big_binary_file::~big_binary_file()
	{
		if(fileopened)
			CloseHandle(hFile);
	}


	#else
	// Linux-Implementation
	
	big_binary_file::big_binary_file(const std::string &filename)
	{
		this->filename = filename;
		fileopened=false;
	};
		
	bool big_binary_file::open(MODE m)
	{	
		if(fileopened)
			close();

		access_mode = m;
		if(m == READ)
		{
			//open for read
			file = fopen64(filename.c_str(), "rb");
		}else
		{
			//open for write
			file = fopen64(filename.c_str(), "wb");
		}

		fileopened=true;
		return true;
	}
	
	void big_binary_file::close()
	{
		if(fileopened)
			fclose(file);	

		fileopened=false;
	}

	bool big_binary_file::is_open()
	{
		return fileopened;
	}
	
	long long big_binary_file::size()
	{
		if(fileopened)
		{
			off64_t size;

			if(!fseeko64(file, 0, SEEK_END))
			{
  				size = ftello64(file);
	  			fseeko64(file, 0, SEEK_SET);
				return size;
			}
			else
			{
				fseeko64(file, 0, SEEK_SET);
				return -1;
			}

		}
		else
		{
			open(READ);

			off64_t size;
			long long value;

			if(!fseeko64(file, 0, SEEK_END))
			{
  				size = ftello64(file);
	  			fseeko64(file, 0, SEEK_SET);
				value = size;
			}
			else
			{
				fseeko64(file, 0, SEEK_SET);
				value = -1;
			}

			close();

			return value;
		}
	}

	bool big_binary_file::read(unsigned char *targetbuffer,unsigned long num, unsigned long *numread)
	{
		if(fileopened && access_mode == READ)
		{
			if(numread == NULL)
			{
				unsigned long _numread;
				_numread = fread(targetbuffer, 1, num, file);
				return num == _numread;
			}
			else
			{
				*numread = fread(targetbuffer, 1, num, file);
				return  num == *numread;
			} 
		}
		return false;
	}

	bool big_binary_file::write(unsigned char *sourcebuffer,unsigned long num, unsigned long *numwrote)
	{
		if(fileopened && access_mode == WRITE)
		{
			if(numwrote == NULL)
			{
				unsigned long _numwrote;
				_numwrote = fwrite(sourcebuffer, 1, num, file);
				return _numwrote == num;
			}
			else
			{
				if(!fwrite(sourcebuffer, 1, num, file))
					std::cerr << "write error"<<std::endl;		

				return *numwrote == num;
			}
		}
		return false;
	}

	bool big_binary_file::seek(long long index)
	{
		if(fileopened)
		{
			off64_t pos;
			pos = index;

			if(!fseeko64(file, pos, SEEK_SET)) 
				return true;
			else 
				return false;
		}

		return false;
	}

	long long big_binary_file::position()
	{
		if(fileopened)
			return ftello64(file);
		else
			return false;
	}
		
	big_binary_file::~big_binary_file()
	{
		if(fileopened)
			fclose(file);
	}

	#endif

	}
}
