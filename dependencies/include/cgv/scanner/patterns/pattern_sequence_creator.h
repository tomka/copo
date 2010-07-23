#pragma once
#include <cgv/media/image/basic_image.h>
#include <vector>

namespace cgv {
	namespace scanner {
		namespace patterns {

/**
* Base class for a pattern sequence creator.
**/
template <typename T>
class pattern_sequence_creator
{
protected:
	int width, height;
public:

	typedef std::vector<cgv::media::image::basic_image<T> > pattern_sequence;

	///constructor with image dimension
	pattern_sequence_creator(int width,int height)
	{
		this->width=width;
		this->height=width;
	}
	///destructor
	virtual ~pattern_sequence_creator(){}

	///override this function to create a concrete pattern sequence
	virtual pattern_sequence* construct_sequence()=0;

};
		}
	}
}

