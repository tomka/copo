#pragma once
#include <cgv/scene/geometry/point.h>

namespace cgv{
	namespace scene{
		namespace geometry{

/**
 * An axis aligned box, defined by to points: min and max
 */

template<typename T,int N>
class axis_aligned_box
{
public:
	point<T,N> min;
	point<T,N> max;

	axis_aligned_box(){}	
	axis_aligned_box(const point<T,N>& min, const point<T,N> &max)
	{
		this->min=min;
		this->max=max;
	}
	
};

		}
	}
}

