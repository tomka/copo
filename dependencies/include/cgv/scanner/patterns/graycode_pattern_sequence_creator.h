#pragma once
#include <cgv/scanner/patterns/pattern_sequence_creator.h>
#include <cgv/utils/graycode.h>

namespace cgv {
	namespace scanner {
		namespace patterns {

/**
* Horizontal stripe pattern sequence creator. 
**/
template <typename T>
class h_graycode_pattern_sequence_creator : public pattern_sequence_creator<T>
{
	
	T one_col;
	T zero_col;
	int column_width;


public:
	
	///constructor
	h_graycode_pattern_sequence_creator (int width, int height,T one_col,T zero_col,int column_width=1) : pattern_sequence_creator(width,height)
	{
		this->one_col =one_col;
		this->zero_col=zero_col;
		this->column_width=column_width;
	}

	///create horizontal pattern sequence
	pattern_sequence* construct_sequence()
	{
		int n = (int)ceil(log(ceil((float)height/(float)column_width))/log(2.0f));
		
		pattern_sequence* sequence =
			new	pattern_sequence(n);
		
		for(int i = 0; i < n;i++)
		{
			(*sequence)[i]=cgv::media::image::basic_image<T>(width,height);
		}

		for(int x = 0; x < width;x++)
		{
			std::bitset<12> bs(x/column_width);
					
			for(int i = 0; i < n; i++)
			{
				T col = bs[i] == 1 ? one_col : zero_col;
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
* Vertical stripe pattern sequence creator. 
**/
template <typename T>
class v_graycode_pattern_sequence_creator : public pattern_sequence_creator<T>
{
	T one_col;
	T zero_col;
	int row_height;


public:
	
	///constructor
	 v_graycode_pattern_sequence_creator (int width, int height,
		 T one_col,T zero_col,int row_height=1) : pattern_sequence_creator(width,height)
	{
		this->one_col =one_col;
		this->zero_col=zero_col;
		this->row_height=row_height;
	}

	///create vertical graycode pattern sequence
	pattern_sequence* construct_sequence()
	{
		int n = (int)ceil(log(ceil((float)height/(float)row_height))/log(2.0f));
		
		pattern_sequence* sequence =
			new	pattern_sequence(n);
		
		for(int i = 0; i < n;i++)
		{
			(*sequence)[i]=cgv::media::image::basic_image<T>(width,height);
		}

		for(int y = 0; y < height;y++)
		{
			std::bitset<12> bs(y/row_height);
					
			for(int i = 0; i < n; i++)
			{
				T col = bs[i] == 1 ? one_col : zero_col;
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
