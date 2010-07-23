#pragma once
#include <cgv/scene/geometry/point.h>
#include <cgv/scene/geometry/direction.h>

namespace cgv{
	namespace scene{
		namespace geometry{

/**
* A straight line defined by a point and a direction
*/
template<typename T, int N>
class straight_line
{
public:
	point<T,N> position;
	cgv::scene::geometry::direction<T,N> direction;
	
	straight_line()
	{
	}
	///create straight line from two points
	straight_line(point<T,N> p1, point<T,N> p2)
	{
		position = p1;
		direction = p2-p1;
		
	}

	//create straight line from point and direction 
	straight_line(point<T,N> p, cgv::scene::geometry::direction<T,N> d)
	{
		position = p;
		direction = d;
		
	}
};

		}
	}
}

