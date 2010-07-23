#pragma once

#include <cgv/media/image/basic_image.h>
#include <cgv/scene/geometry/point.h>
#include <cgv/scene/geometry/ray.h>
#include <cgv/math/linalg/linalg.h>
#include <polygonal_mesh.h>
#include <types.h>

namespace SnowSim{
	namespace Utilities{
		namespace Intersect{

			/// return true if the plane in which face f lies
			/// and ray r are intersecting.
			/// To allow intersections in negative ray direction set neg_t_allow true.
			bool intersect(	const SnowSim::polygonal_mesh::face *f,
							const ray3f& r, bool neg_t_allowed = false);


			// tests if the ray intersects the plane in which f lies.
			// if so, intersection-parameter is filled with coordinates
			// of this point.
			bool intersect(	const SnowSim::polygonal_mesh::face *f,
				const ray3f& r, bool neg_t_allowed, point3f& intersection);

			// tests if the ray intersects the plane in which f lies.
			// if so, intersection-parameter is filled with coordinates
			// of this point.
			bool intersect(	const SnowSim::polygonal_mesh::face *f,
				const ray3f& r, bool neg_t_allowed, PrecisionType& param_t, point3f& intersection);

			// tests if the ray from p1 to p2 intersects the triangle spanned by edge1 and edge2.
			// where both edges are starting at the same point "start".
			// if so, intersection-parameter is filled with coordinates
			// of this point. True is only returned if there is an intersection
			// AND if segment = true the intersection lies between p1 and p2
			bool intersect(const point3f& start, const point3f& edge1, const point3f& edge2, const point3f& p1, const point3f& p2,
				bool segment, bool neg_t_allowed, PrecisionType& t_result);

			// tests if the ray intersects the plane in which f lies.
			// if so, intersection-parameter is filled with coordinates
			// of this point.
			/// param_t is filled with the resulting parameter t in a line p1 + t * (p2 - p1)
			bool intersect(	const SnowSim::polygonal_mesh::face *f, const point3f& p1, const point3f& p2,
				bool segment, PrecisionType& param_t, bool neg_t_allowed, point3f& intersection);

			// tests if the ray from p1 to p2 intersects the face f.
			// if so, intersection-parameter is filled with coordinates
			// of this point. True is only returned if there is an intersection
			// AND if segment = true the intersection lies between p1 and p2
			bool intersect(	const SnowSim::polygonal_mesh::face *f, const point3f& p1, const point3f& p2,
				bool segment, bool neg_t_allowed, point3f& intersection);
		}
	}
}
