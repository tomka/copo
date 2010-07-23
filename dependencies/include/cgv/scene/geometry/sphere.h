#pragma once
#include <cgv/scene/geometry/point.h>

namespace cgv{
	namespace scene{
		namespace geometry{

/**
 * A sphere data structure. The sphere is defined by its central point and a radius
 */
template <typename T,int N>
class sphere
{
public:
	point<T,N> center;
	T radius;
	///create a sphere
	sphere()
	{
		center.set(0,0,0);
		radius = 1;
	}

	sphere(const point<T,N>& c, const T& radius)
	{
		center=c;
		this->radius=radius;
	}
};

		}
	}
}

