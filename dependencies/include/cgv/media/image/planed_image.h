#pragma once
#include <math.h>
#include <cgv/type/size_type.h>
#include <cgv/media/image/basic_image.h>

namespace cgv {
	namespace media{
		namespace image{
/**
* A simple image class.
*/
	/// 
	using cgv::type::uint32;

template<typename T, int vstep1 = 1, int hstep1 = 1, int vstep2 = 2, int hstep2 = 2, int vstep3 = 2, int hstep3 = 2>
class planed_image : public basic_image<T>
{
public:
	static int vstep(int p) { static const int vs[] = { vstep1, vstep2, vstep3 }; return vs[p]; }
	static int hstep(int p)  { static const int hs[] = { hstep1, hstep2, hstep3 }; return hs[p]; }
	static const int nr_planes = 3;
	/// type of this planed image
	typedef planed_image<T,vstep1,hstep1,vstep2,hstep2,vstep3,hstep3> this_type;
	/// type of base image
	typedef basic_image<T> base_type;
	/// keep three child images that do not manage memory
	base_type planes[3];
	/// return size of T-plane of the i-th color component, where i starts from 0
	uint32 size(int i) const
	{
		return planes[i].width*planes[i].height;
	}
	/// total number of stored elements of type T
	uint32 size() const
	{
		return size(0)*size(1)*size(2);
	}
	/// set the data pointer to external data
	void set_data_ptr(T* data, bool dealloc = false)
	{
		base_type::set_data_ptr(data, dealloc);
		planes[0].set_data_ptr(data);
		for (int p=1; p<nr_planes; ++p)
			planes[p].set_data_ptr(planes[p-1].data_ptr()+size(p-1));
	}

	void construct(bool alloc)
	{
		for (int p=0; p<nr_planes; ++p) {
			planes[p].width = width/vstep(p);
			planes[p].height = height/hstep(p);
		}
		if (alloc) {
			planes[0].set_data_ptr(data = new T[size()]);
			for (int p=1; p<nr_planes; ++p)
				planes[p].set_data_ptr(planes[p-1].data_ptr()+size(p-1));
			dealloc = true;
		}
	}
	///construct an image with given size
	planed_image(int width, int height, bool alloc) : basic_image(width,height,false)
	{
		construct(alloc);
	}
	
	///construct an image with given size, all pixels are set to the three initial values
	planed_image(T iv1, T iv2, T iv3, int width, int height) : basic_image(width,height,false)
	{
		T iv[] = { iv1, iv2, iv3 };
		construct(true);
		for (int p=0; p<nr_planes; ++p)
			std::fill(planes[p].data_ptr(), planes[p].data_ptr()+size(p),iv[p]);
	}

	///copy constructor
	planed_image(const this_type& image)
	{
		width = image.width;
		height = image.height;
		construct(true);
		std::copy(data,data+size(),image.data);
	}
	
	/// access pixel plane
	T& operator()(int x,int y, int p,bool clamp=false)
	{
		return planes[p](x/vstep(p),y/hstep(p),clamp);
	}

	///linear interpolated subpixel plane read access
	T operator()(float x, float y, int p, bool clamp = false)
	{
		return planes[p](x/vstep(p),y/hstep(p),clamp);
	}
	
	///copy by assign
	this_type& operator=(const this_type& image)
	{
		if (size() != image.size()) 
		{
			deallocate();
			width = image.width;
			height = image.height;
			construct();
		}
		std::copy(data,data+size(),image.data);
		return *this;
	}
};
		}
	}
}