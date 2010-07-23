#pragma once
#include <cgv/streamprocessing/streamprocessing.h>
#include <cgv/streamprocessing/imaging_nodes/raw_image.h>
#include <ctime>
#include <math.h>


using namespace cgv::streamprocessing;

namespace cgv {
	namespace streamprocessing {
		namespace imaging_nodes {

template <typename patterntype, int N>
class pattern_generator_node : public stream_node
{
public:
	port_out<patterntype> output;

	typedef typename patterntype::value_type pixel_type;
	

private:
	patterntype *pattern;

	

public:

	pattern_generator_node():stream_node()
	{
		register_port_out("pattern",&output);
		
		pattern = new patterntype();

	}
		
	virtual ~pattern_generator_node()
	{
		delete pattern;				
	}

	virtual pixel_type create_pattern(int i, int x, int y)=0;

	
	void run()
	{
		std::cout << "start pattern generator node\n";
		int i = 0;
		while(no_stop_request() && i < N)
		{
			
			for(int x = 0; x < pattern->width;x++)
			{
				for(int y = 0; y < pattern->height;y++)
				{
					(*pattern)(x,y) = create_pattern(i,x,y);
				}
			}
			output << &pattern;
			i++;			
		}
		patterntype* null = NULL;
		output << &null;
		std::cout << "stop pattern generator node\n";
	}

		
};



template <int N>
class lineshift_pattern_generator_node : public pattern_generator_node<projector_image_type, N>
{
public:
	

	pixel_type create_pattern(int i, int x, int y)
	{
		return ((x-i)%N==0)? 255 : 0;
	}
	
};

template <int N=1>
class stripe_pattern_generator_node : public pattern_generator_node<projector_image_type, N>
{
	int stripe_width, gap_width;

public:

	
	stripe_pattern_generator_node(int stripe_width, int gap_width)
	{
		this->stripe_width = stripe_width;
		this->gap_width = gap_width;
	}

	pixel_type create_pattern(int i, int x, int y)
	{
		return (x%(stripe_width+gap_width) < stripe_width)? 255 : 0;
	}
	
};


template <int N>
class graycode_pattern_generator_node : public pattern_generator_node<projector_image_type, N>
{
public:
	

	pixel_type create_pattern(int i, int x, int y)
	{
		unsigned int code,w;
	
		
		w = (x)/(projector_image_type::width /(1<<N));
	
		code = (w) ^ ( w /2 );
		
		
		return ( ((1 << (N-1-i))  & code) ? 255 : 0);
	}
	
};

template <int N=1>
class on_pattern_generator_node : public pattern_generator_node<projector_image_type, N>
{
public:
	
	pixel_type create_pattern(int i, int x, int y)
	{		
		return 255;
	}
};

template <int N=1>
class random_pattern_generator_node : public pattern_generator_node<projector_image_type, N>
{
	unsigned char *data;
	int w,h,dx,dy;
	
public:
	random_pattern_generator_node(int dx,int dy)
	{
		std::srand(unsigned( time(0)));
		w = (int)ceil(1024.0/dx);
		h = (int)ceil(768.0/dy);
		this->dx=dx;
		this->dy=dy;
		
		data = new unsigned char[w*h];
		for(int x =0; x < w;x++)
			for(int y = 0;y < h;y++)
			{
				data[x+y*w]= int( ( (double)std::rand() / (RAND_MAX + 1.0 ) )* 255 );
			}
	}

	
	pixel_type create_pattern(int i, int x, int y)
	{		
		return data[x/dx+(y/dy)*w];

	}
};

template <int N=1>
class salt_noise_generator_node : public pattern_generator_node<projector_image_type, N>
{
	unsigned char *data;
	int w,h,dx,dy;
	
public:
	salt_noise_generator_node(int dx,int dy, int p=13)
	{
		std::srand(unsigned( time(0)));
		w = (int)ceil(1024.0/dx);
		h = (int)ceil(768.0/dy);
		this->dx=dx;
		this->dy=dy;
		data = new unsigned char[w*h];
		for(int x =0; x < w; x++)
		{
			for(int y = 0;y < h; y++)
			{
				data[x+y*w]=0;
			}
		}

		for(int x =0; x < w; x++)
			for(int y = 0;y < h; y++)
			{
				
				
					int r = (int)(100*((float) std::rand())  / (RAND_MAX + 1.0f ) ); 
					if(r < p)
					{
						for(int i= -1; i < 1; i++)
						{
							for(int j= -1; j <= 1; j++)
							{
								if( x+i >= 0 && x+i < w && (j+y) >= 0 && (j+y) < h)
								{
									if((i == 0) ^( j==0))
										data[(x+i)+(j+y)*w]=255;
									else
										data[(x+i)+(j+y)*w]=1;
								}
							}
						}
					
					}
				
					

			}
			for(int x =0; x < w; x++)
			{
				for(int y = 0;y < h; y++)
				{
					if(x%30 == 0 ||y%30 == 0)
					{
						if(x%60 == 0 ||y%60 == 0)
							data[x+y*w]=255;
						else
							data[x+y*w]=0;

					}
					
				}
			}
			
	}



	
	pixel_type create_pattern(int i, int x, int y)
	{
		return data[x/dx+(y/dy)*w];
	}
};

template <int N=1>
class off_pattern_generator_node : public pattern_generator_node<projector_image_type, N>
{
public:
	
	pixel_type create_pattern(int i, int x, int y)
	{		
		return 0;
	}
	
};



		}
	}
}