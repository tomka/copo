#pragma once

#include <cgv/scene/geometry/normal.h>

namespace cgv{
	namespace scene{
		namespace geometry{

/**
* A plane  ax+by+cz+d = 0 stored as dot(normal,(x,y,z)^N)-distance=0
*/
template <class T,int N>
class plane
{
public:
	cgv::scene::geometry::normal<T,N> normal;
	T distance;



	plane(){}

	plane(const cgv::scene::geometry::normal<T,N> &n, const T &distance)
	{
		this->normal = n;
		this->distance=distance;
	}
};

		}
	}
}

