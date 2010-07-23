#pragma once
#include <cgv/scanner/patterns/pattern_sequence_creator.h>

namespace cgv {
	namespace scanner {
		namespace patterns {

/**
* Horizontal stripe pattern sequence creator. 
**/
template <typename T>
class h_stripe_pattern_sequence_creator : public pattern_sequence_creator<T>
{
	int stripe_width;
	int gap_width;
	T stripe_col;
	T gap_col;


public:
	
	///constructor
	 h_stripe_pattern_sequence_creator (int width, int height,int stripe_width, 
		 int gap_width,T stripe_col,T gap_col) : pattern_sequence_creator(width,height)
	{
		this->stripe_width=stripe_width;
		this->gap_width=gap_width;
	}

	///create stripe pattern sequence
	pattern_sequence* construct_sequence()
	{
		pattern_sequence* sequence =
			new	pattern_sequence(1);
		
		(*sequence)[0]=cgv::media::image::basic_image<T>(width,height);
		
		for(int x = 0; x < width;x++)
		{
			T col = (x%(stripe_width+gap_width) < stripe_width)? 
						stripe_col : gap_col;

			for(int y = 0; y < height;y++)
			{
			
				(*sequence)[0](x,y) = col;
			}
		}

		
		return sequence;
		
	}

};

/**
* Vertical stripe pattern sequence creator. 
**/
template <typename T>
class v_stripe_pattern_sequence_creator : public pattern_sequence_creator<T>
{
	int stripe_width;
	int gap_width;
	T stripe_col;
	T gap_col;


public:
	
	///constructor
	 v_stripe_pattern_sequence_creator (int width, int height,int stripe_width, 
		 int gap_width,T stripe_col,T gap_col) : pattern_sequence_creator(width,height)
	{
		this->stripe_width=stripe_width;
		this->gap_width=gap_width;
	}

	///create stripe pattern sequence
	pattern_sequence* construct_sequence()
	{
		pattern_sequence* sequence =
			new	pattern_sequence(1);
		
		(*sequence)[0]=cgv::media::image::basic_image<T>(width,height);
		
		for(int y = 0; y < height;y++)
		{
			T col = (y%(stripe_width+gap_width) < stripe_width)? 
						stripe_col : gap_col;
			for(int x = 0; x < width;x++)
			{
				(*sequence)[0](x,y) = col;
			}
		}

		
		return sequence;
		
	}

};
		}
	}
}
