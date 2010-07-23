#pragma once
#include <cgv/scene/geometry/point.h>
#include <cgv/scene/geometry/direction.h>

namespace cgv{
	namespace scene{
		namespace geometry{

			/**
			* A simple ray data structure. The ray is defined by its origin point and a direction.
			*/
			template<class T,int N>
			class ray
			{
				private:
					ray()
						{
						}
						
				public:
					point<T,N> origin;
					cgv::scene::geometry::direction<T,N> direction;
					
					///create a ray 
					ray(const point<T,N>& o,const cgv::scene::geometry::direction<T,N> &d)
					{
						origin=o;
						direction=d;
					}
			};

		}
	}
}

