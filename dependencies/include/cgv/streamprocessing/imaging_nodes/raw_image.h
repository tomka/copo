#pragma once

namespace cgv {
	namespace streamprocessing {
		namespace imaging_nodes {

template<typename T,int W,int H, int C=1>
struct raw_image
{
	T data[W*H*C];

	static const int width = W;
	static const int height = H;
	static const int channels = C;
	static const int count =W*H*C;
	static const int size_in_bytes =sizeof(T)*count;

	typedef T value_type;

	raw_image()
	{
	}
	
	raw_image(T initialvalue)
	{
		for(int i = 0; i < count;i++)
			data[i]=initialvalue;
	}

	raw_image(const raw_image& image)
	{
		memcpy(data,image.data,size_in_bytes);
	}
		
	
	T& operator()(int x, bool clamp=false)
	{
		//clamp
		if(clamp)
		{
			x= x < 0? 0:x;
			x= x >= width? width-1:x;
		}
		
		return data[x];
	}
	T& operator()(int x,int y,bool clamp=false)
	{
		//clamp
		if(clamp)
		{
			x= x < 0? 0:x;
			x= x >= width? width-1:x;
			y= y < 0? 0:y;
			y= y >= height? height-1:y;
		}
		
		return data[width*y+x];
	}
	T& operator()(int x, int y,int z,bool clamp=false)
	{
		//clamp
		if(clamp)
		{
			x= x < 0? 0:x;
			x= x >= width? width-1:x;
			y= y < 0? 0:y;
			y= y >= height? height-1:y;
			z= z < 0? 0:z;
			z =	z >=channels?channels-1:z;
		}

		return data[z*(width*height)+width*y+x];
	}

	raw_image& operator=(const raw_image& image)
	{
		memcpy(data,image.data,size_in_bytes);
		return *this;
	}


};

template<typename T,int W,int H, int C=1>
struct filter_mask : public raw_image<T,W,H,C>
{
	static const int center_x = W/2;
	static const int center_y = H/2;
};



typedef raw_image<unsigned char, 1024,768> projector_image_type;
typedef raw_image<unsigned char,1280,1024> camera_image_type_1280x1024;
typedef raw_image<unsigned char,512,512> camera_image_type_512x512;
typedef raw_image<unsigned char,128,128> camera_image_type_128x128;
typedef raw_image<unsigned char,640,480> camera_image_type_640x480;
typedef raw_image<unsigned char,320,200> camera_image_type_320x200;
typedef raw_image<short,1280,1024> encoded_camera_image_type_1280x1024;




typedef filter_mask<float,3,3> filter_mask_33f;
typedef filter_mask<float,1,5> filter_mask_15f;
typedef filter_mask<float,1,3> filter_mask_13f;

typedef filter_mask<unsigned char,3,3> correlation_mask_33ub;
typedef filter_mask<unsigned char,6,6> correlation_mask_66ub;
typedef filter_mask<unsigned char,3,6> correlation_mask_36ub;

		}
	}
}