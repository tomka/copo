#pragma once 
#include <cgv/media/image/basic_image.h>
#include <cgv/scene/geometry/point.h>
#include <cgv/scene/geometry/plane.h>
#include <cgv/scene/geometry/ray.h>
#include <cgv/math/linalg/linalg.h>


namespace cgv{
	namespace scene{
		namespace intersect{

/// return true if plane pl and ray r are intersecting. The intersection point is stored
/// in p. To allow intersections in negative ray direction set neg_t_allow true.
bool intersect(const cgv::scene::geometry::plane<float,3> &pl,
			   const cgv::scene::geometry::ray<float,3>& r,
			   cgv::scene::geometry::point<float,3>* p,
			   bool neg_t_allowed=false)
{
	
	
	float dot_dn = dot(r.direction,pl.normal);

	//parallel
	if(dot_dn == 0.0f)
		return false;

	float t = -(dot(r.origin,pl.normal)-pl.distance)/dot_dn;
	if(t>=0 || neg_t_allowed)
		*p = r.origin+t*r.direction;
	
	return true;

}

/*
//return number of intersections (0,1,2) ray/sphere intersection
int intersect(const cgv::scene::geometry::sphere<float,3> &s,
			   const cgv::scene::geometry::ray<float,3>& r,
			   cgv::scene::geometry::point<float,3>* p1,
			   cgv::scene::geometry::point<float,3>* p2,
			   bool neg_t_allowed=false)
{
	T a = dot(r.direction,r.direction);
	T b = 2*dot(r.direction,r.origin - s.center);
	T c = dot(s.center,s.center) + dot(r.origin,r.origin) -2*dot(s.center,r.origin)-s.radius*s.radius;
	T discriminant = b*b -4*a*c;
	if(discriminant < 0)
		return 0;
	T ro = sqrt(discriminant);
	T t1 = (-b-ro)/(2*a);
	*p1 = r.origin +t1*r.direction;
	int N =0;
	if(t1 >= 0 || neg_t_allowed)
		N++;
	if(discriminant == 0 )		
			return 1;
	//discriminant > 0 
	T t2 = (-b+ro)/(2*a);
	if(N==0 )
	{
		if(t2 >= 0)
		{
			*p1 = r.origin +t2*r.direction;
			return 1;
		}
		else 
			return 0;
	}
	if(t2>= 0 ||neg_t_allowed)
	{
		*p2 = r.origin + t2*r.direction;
		return 2;
	}
	else
		return 1;
		
		 
}

//ray-box intersection
//ray-quadric intersection

*/

		}
	}
}
