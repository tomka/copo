#pragma once

#include <cgv/media/image/basic_image.h>
#include <cgv/utils/big_binary_file.h>



namespace cgv {
	namespace media{
		namespace image{



template<typename T>
class basic_image_stream
{
	//internal file handler
	cgv::utils::big_binary_file *file;

protected:
	///pixel width of images in file
	int width;
	///pixel height of images in file
	int height;

public:
	enum open_mode { READ, WRITE };

	///constructor associates the reader with a given filename and an optional image size
	basic_image_stream(int width=1280, int height=1024)
	{
		
		this->width = width;
		this->height = height;
	}


	///destructor
	virtual ~basic_image_stream()
	{
		delete file;
	}

	/// set the image dimensions
	void set_size(int w, int h)
	{
		width=w;
		height=h;
	}


	///open file
	bool open(const std::string& file_name, open_mode mode)
	{
		file = new cgv::utils::big_binary_file(file_name);
		if(mode == READ)
			return file->open(cgv::utils::big_binary_file::READ);
		else
			return file->open(cgv::utils::big_binary_file::WRITE);
	}

	///open file
	void close()
	{
		file->close();
	}

	///return number of pictures in vds file
	unsigned long size()
	{
		return (unsigned long)(file->size()/(width*height*sizeof(T)));
	}


	///set position in vds file with picture granularity
	bool seek(long long pictureindex)
	{
		if(file->is_open())
		{
			return file->seek((long long)(pictureindex*width*height*sizeof(T)));
		}
		return false;
	}

	///read image from current position in file
	bool read(cgv::media::image::basic_image<T> *image)
	{
		if(image->width == width && image->height == height)
		{

			return file->read((unsigned char*)image->data_ptr(),(unsigned long)(width*height*sizeof(T)));
		}
		return false;
	}

	///write image from current position in file
	bool write(cgv::media::image::basic_image<T> *image)
	{
		if(image->width == width && image->height == height)
		{

			return file->write((unsigned char*)image->data_ptr(),(unsigned long)(width*height*sizeof(T)));
		}
		return false;
	}


};
		}
	}
}