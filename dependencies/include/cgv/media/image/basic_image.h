#pragma once
#include <math.h>

namespace cgv {
	namespace media{
		namespace image{
/**
* A simple image class.
*/
template<typename T>
class basic_image
{
protected:
	///image data
	T *data;
	bool dealloc;
public:
	///image dimension
	int width,height;


	/// return pointer to image data
	T* data_ptr()
	{
		return data;
	}
	/// return const pointer to image data
	const T* data_ptr() const
	{
		return data;
	}

	/// set the data pointer to external data
	virtual void set_data_ptr(T* _data, bool _dealloc = false)
	{
		data = _data;
		dealloc = _dealloc;
	}
	/// resize the image and return whether the size changed
	bool resize(int new_width, int new_height)
	{
		if (width == new_width && height == new_height)
			return false;
		deallocate();
		width = new_width;
		height = new_height;
		data = new T[width*height];
		dealloc = true;
		return true;
	}
	/// standard constructor necessary for arrays
	basic_image() : data(0), dealloc(false), width(0), height(0) {}

	///construct an image with given size
	basic_image(int width, int height, bool alloc=true)
	{
		this->width=width;
		this->height=height;
		if (alloc) {
			data = new T[width*height];
			dealloc = true;
		}
		else {
			data = 0;
			dealloc = false;
		}
	}
	///construct an image with given size, all pixels are set to initialvalue
	basic_image(T initialvalue,int width, int height)
	{
		this->width=width;
		this->height=height;
		data = new T[width*height];
		dealloc = true;
		for(int i = 0; i < width*height;i++)
			data[i]=initialvalue;
	}

	///copy constructor
	basic_image(const basic_image<T>& image)
	{
		
		width = image.width;
		height = image.height;
		dealloc = true;
		data = new T[width*height];
		memcpy(data,image.data,sizeof(T)*width*height);
	}
	virtual void deallocate()
	{
		if (dealloc)
			delete[] data;
		dealloc = false;
		data = 0;
	}

	///destructor
	virtual ~basic_image()
	{
		deallocate();
	}
	
	
		
	
	///unsave access to  pixel 
	T& operator()(int x,int y,bool clamp=false)
	{
		//clamp
		if(clamp)
		{
			x= x >= width? width-1:x;
			x= x < 0? 0 : x;
			y= y >= height? height-1:y;
			y= y < 0? 0 : y;
		}
		
		return data[width*y+x];
	}

	//save pixel access (with index border test)
	void set_pixel(int x,int y,const T &color)
	{
		if(x < 0 || x >= width || y < 0 || y >= height)
			return;
		data[width*y+x]=color;
	}

	///linear interpolated subpixel read access
	T operator()(float x, float y, bool clamp) 
	{
		int x1 = (int)x;
		int y1 = (int)y;
		int x2 = x1+1;
		int y2 = y1+1;
		float dx =x-x1;
		float dy =y-y1;
		if(clamp)
		{
			x1= x1 < 0? 0:x1;
			x1= x1 >= width? width-1:x1;
			y1= y1 < 0? 0:y1;
			y1= y1 >= height? height-1:y1;
			x2= x2 < 0? 0:x2;
			x2= x2 >= width? width-1:x2;
			y2= y2 < 0? 0:y2;
			y2= y2 >= height? height-1:y2;
		}
		
		

		return (T)((1-dy)*((1-dx)*data[width*y1+x1] + dx*data[width*y1+x2]) + 
			dy*((1-dx)*data[width*y2+x1] + dx*data[width*y2+x2]));
	}
	
	///copy by assign
	basic_image& operator=(const basic_image& image)
	{
		if(width*height != image.width*image.height) 
		{
			delete []data;
			width = image.width;
			height = image.height;
			data = new T[width*height];
		}
		
		memcpy(data,image.data,sizeof(T)*width*height);
		return *this;
	}
	


};
		}
	}
}

