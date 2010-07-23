#pragma once
#include <cgv/scanner/patterns/pattern_sequence_creator.h>

namespace cgv {
	namespace scanner {
		namespace patterns {

/**
* Horizontal lineshift pattern sequence creator. 
**/
template <typename T>
class h_lineshift_pattern_sequence_creator : public pattern_sequence_creator<T>
{
	int stripe_width;
	int gap_width;
	int shift_width;
	int nimg;
	T stripe_col;
	T gap_col;


public:
	
	///constructor
	 h_lineshift_pattern_sequence_creator (int width, int height, 
		 int stripe_width,int gap_width,int shift_width,int nimg,
		 T stripe_col,T gap_col) : pattern_sequence_creator(width,height)
	{
		this->stripe_width=stripe_width;
		this->gap_width=gap_width;
		this->shift_width = shift_width;
		this->nimg = nimg;
	}

	///create lineshift pattern sequence
	pattern_sequence* construct_sequence()
	{

		

		pattern_sequence* sequence =
			new	pattern_sequence(nimg);
		
		for(int i = 0; i < nimg; i++)
			(*sequence)[i]=cgv::media::image::basic_image<T>(width,height);
		
		
		for(int i = 0; i < nimg; i++)
		{
			for(int x = 0; x < width;x++)
			{
				T col = ((x+i*shift_width)%(stripe_width+gap_width) < stripe_width)? 
							stripe_col : gap_col;

				for(int y = 0; y < height;y++)
				{
				
					(*sequence)[i](x,y) = col;
				}
			}
		}

		
		return sequence;
		
	}

};

/**
* Vertical lineshift pattern sequence creator. 
**/
template <typename T>
class v_lineshift_pattern_sequence_creator : public pattern_sequence_creator<T>
{
	int stripe_height;
	int gap_height;
	int shift_height;
	int nimg;
	T stripe_col;
	T gap_col;


public:
	
	///constructor
	 v_lineshift_pattern_sequence_creator (int width, int height,int stripe_height, 
		 int gap_height, int shift_height,int nimg,T stripe_col,T gap_col) : pattern_sequence_creator(width,height)
	{
		this->stripe_height=stripe_height;
		this->gap_height=gap_height;
		this->shift_height = shift_height;
		this->nimg = nimg;
	}

	///create lineshift pattern sequence
	pattern_sequence* construct_sequence()
	{
		pattern_sequence* sequence =
			new	pattern_sequence(nimg);
		
		for(int i = 0; i < nimg; i++)
			(*sequence)[i]=cgv::media::image::basic_image<T>(width,height);
		
		
		for(int i = 0; i < nimg; i++)
		{
			for(int y = 0; y < height;y++)
			{
				T col = ((y+i)%(stripe_height+gap_height) < stripe_height)? 
							stripe_col : gap_col;
				for(int x = 0; x < width;x++)
				{
					(*sequence)[i](x,y) = col;
				}
			}
		}

		
		return sequence;
		
	}

};
		}
	}
}
