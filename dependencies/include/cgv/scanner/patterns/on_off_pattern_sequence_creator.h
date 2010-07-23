#pragma once
#include <cgv/scanner/patterns/pattern_sequence_creator.h>

namespace cgv {
	namespace scanner {
		namespace patterns {

/**
* On Off pattern sequence creator.
**/
template <typename T>
class on_off_pattern_sequence_creator : public pattern_sequence_creator<T>
{
	T on,off;
public:
	
	on_off_pattern_sequence_creator (int width, int height,const T on ,const T off): pattern_sequence_creator(width,height)
	{
		this->on = on;
		this->off = off;
	}
	
	pattern_sequence* construct_sequence()
	{
		pattern_sequence* sequence = 
			new	pattern_sequence(2);
		
		(*sequence)[0]=cgv::media::image::basic_image<T>(on,width,height);
		(*sequence)[1]=cgv::media::image::basic_image<T>(off,width,height);
		return sequence;
	}

};
		}
	}
}