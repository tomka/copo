#pragma once
#include <cgv/scene/geometry/point.h>

namespace cgv{
	namespace scene{
		namespace geometry{

/**
 * A box defined by its center, extent (width,height,depth) and orientation.
 */

template<typename T,int N>
class box 
{
public:

	cgv::linalg::math::fix_sqr_mat<T,N> orientation;
	cgv::linalg::math::fix_diag_mat<T,N> extent;
	point<T,N> center;
	
	

	box():extent(0.5),center(0)
	{
		orientation = cgv::linalg::math::identity<T,N>();
	}


	
};

		}
	}
}