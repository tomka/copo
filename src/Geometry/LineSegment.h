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
class line_segment
{
	private:
		line_segment()
		{
		}

	public:
		point<T,N> start;
		point<T,N> end;
		direction<T,N> dir;


		///create straight line from two points
		line_segment(point<T,N> p1, point<T,N> p2)
		{
			start = p1;
			end = p2;
			dir = p2-p1;

		}

		//create straight line from point and direction
		line_segment(point<T,N> p, cgv::scene::geometry::direction<T,N> d, float t)
		{
			start = p;
			dir = d;
			end = point<T,N>(p+d*t);
		}

		point<T,N>& getStart() {
			return start;
		}

		point<T,N>& getEnd() {
			return end;
		}

		direction<T,N>& getDirecion() {
			return dir;
		}
};
		}
	}
}

