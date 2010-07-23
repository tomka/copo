#pragma once

#include "Intersection.h"
#include "types.h"
#include <Exceptions/NotImplementedException.h>

namespace SnowSim{
	namespace Utilities{
		namespace Intersect{

			/// return true if the plane in which face f lies
			/// and ray r are intersecting.
			/// To allow intersections in negative ray direction set neg_t_allow true.
			bool intersect(	const SnowSim::polygonal_mesh::face *f,
				const ray3f& r, bool neg_t_allowed /*= false*/)
			{
				// get the parameter for r to get to the intersection point
				float nominator = dot( f->normal, r.direction );

				if (nominator < NearlyZero && nominator > -NearlyZero)
					return false;

				float t = dot( -f->normal, f->m_halfedge->origin->position ) / nominator;

				if (!neg_t_allowed && t < NearlyZero)
					return false;

				return true;

			}

			// tests if the ray intersects the plane in which f lies.
			// if so, intersection-parameter is filled with coordinates
			// of this point.
			bool intersect(	const SnowSim::polygonal_mesh::face *f,
				const ray3f& r, bool neg_t_allowed, point3f& intersection)
			{
				// get the parameter for r to get to the intersection point
				float nominator = dot( f->normal, r.direction );

				if (nominator < NearlyZero && nominator > -NearlyZero)
					return false;

				float t = dot( -f->normal, r.origin - f->m_halfedge->origin->position ) / nominator;

				if (!neg_t_allowed && t < NearlyZero)
					return false;

				point3f result = r.origin + t * r.direction;

				intersection.x() = result.x();
				intersection.y() = result.y();
				intersection.z() = result.z();
				intersection.w() = result.w();

				return true;
			}

			// tests if the ray intersects the plane in which f lies.
			// if so, intersection-parameter is filled with coordinates
			// of this point.
			bool intersect(	const SnowSim::polygonal_mesh::face *f,
				const ray3f& r, bool neg_t_allowed, PrecisionType& param_t, point3f& intersection)
			{
				// get the parameter for r to get to the intersection point
				float nominator = dot( f->normal, r.direction );

				if (nominator < NearlyZero && nominator > -NearlyZero)
					return false;

				float t = dot( -f->normal, r.origin - f->m_halfedge->origin->position ) / nominator;

				if (!neg_t_allowed && t < NearlyZero)
					return false;

				point3f result = r.origin + t * r.direction;

				param_t = t;

				intersection.x() = result.x();
				intersection.y() = result.y();
				intersection.z() = result.z();
				intersection.w() = result.w();

				return true;
			}


			#define TEST_CULL

			// tests if the ray from p1 to p2 intersects the triangle spanned by edge1 and edge2
			// (edge1 is the "right" one in a CCW-Triangle)
			// where both edges are starting at the same point "start".
			// if so, intersection-parameter is filled with coordinates
			// of this point. True is only returned if there is an intersection
			// AND if segment = true the intersection lies between p1 and p2
			bool intersect(const point3f& start, const point3f& edge1, const point3f& edge2, const point3f& p1, const point3f& p2,
				bool segment, bool neg_t_allowed, PrecisionType& t_result)
			{

				double t, u, v;

				point3f dir = p2 - p1;

				/* begin calculating determinant - also used to calculate U parameter */
				point3f pvec = cross(dir, edge2);

				/* if determinant is near zero, ray lies in plane of triangle */
				double det = dot(edge1, pvec);

				#ifdef TEST_CULL /* define TEST_CULL if culling is desired */

					if (det < NearlyZero)
						return false;

					/* calculate distance from vert0 to ray origin */
					point3f tvec = p1 - start;

					/* calculate U parameter and test bounds */
					u = dot(tvec, pvec);
					if (u < 0.0 || u > det)
						return false;

					/* prepare to test V parameter */
					point3f qvec = cross(tvec, edge1);

					/* calculate V parameter and the bounds */
					v = dot(dir, qvec);
					if (v < 0.0 || u + v > det)
						return false;

					/* calculate t, scale parameters, ray intersection triangle */
					t = dot(edge2, qvec);
					double inv_det = 1.0 / det;
					t *= inv_det;
					u *= inv_det;
					v *= inv_det;

				#else /* the non-culling branch */

					if (det > -NearlyZero && det < NearlyZero)
						return false;
					float inv_det = 1.0 / det;

					/* calculaet distance from he->origin to ray origin */
					point3f tvec = p1 - he->origin->position;

					/* calculate U parameter and test bounds */
					u = dot(tvec, pvec) * inv_det;
					if (u < 0.0 || u > 1.0)
						return false;

					/* prepare for test V parameter */
					point3f qvec = cross(tvec, edge1);

					/* calculate V parameter and test bounds */
					v = dot(dir, qvec) * inv_det;
					if (v < 0.0 || u + v > 1.0)
						return false;

					/* calculate t, ray intersects triangle */
					t = dot(edge2, qvec) * inv_det;

				#endif

				t_result = t;

				/* check if negative t is allowed */
				if (!neg_t_allowed && t < 0.0)
					return false;

				/* check for segment test */
				if ( segment && (t < 0.0 || t > 1.0) )
					return false;

				return true;
			}

			/// tests if the ray intersects the plane in which f lies.
			/// if so, intersection-parameter is filled with coordinates
			/// of this point.
			/// param_t is filled with the resulting parameter t in a line p1 + t * (p2 - p1)
			bool intersect(	const SnowSim::polygonal_mesh::face *f, const point3f& p1, const point3f& p2,
				bool segment, PrecisionType& param_t, bool neg_t_allowed, point3f& intersection)
			{
				if (f->valence != 3)
					throw Exceptions::NotImplementedException("Not implemented: intersect for polygon wiht > 3 vertices");

				double t, u, v;

				point3f dir = p2 - p1;

				SnowSim::polygonal_mesh::halfedge* he = f->m_halfedge;

				/* find vectors for two edges sharing he->origin */
				point3f edge1 = he->next->origin->position - he->origin->position;
				point3f edge2 = he->prev->origin->position - he->origin->position;

				/* begin calculating determinant - also used to calculate U parameter */
				point3f pvec = cross(dir, edge2);

				/* if determinant is near zero, ray lies in plane of triangle */
				double det = dot(edge1, pvec);

				#ifdef TEST_CULL /* define TEST_CULL if culling is desired */

					if (det < NearlyZero)
						return false;

					/* calculate distance from vert0 to ray origin */
					point3f tvec = p1 - he->origin->position;

					/* calculate U parameter and test bounds */
					u = dot(tvec, pvec);
					if (u < 0.0 || u > det)
						return false;

					/* prepare to test V parameter */
					point3f qvec = cross(tvec, edge1);

					/* calculate V parameter and the bounds */
					v = dot(dir, qvec);
					if (v < 0.0 || u + v > det)
						return false;

					/* calculate t, scale parameters, ray intersection triangle */
					t = dot(edge2, qvec);
					double inv_det = 1.0 / det;
					t *= inv_det;
					u *= inv_det;
					v *= inv_det;

				#else /* the non-culling branch */

					if (det > -NearlyZero && det < NearlyZero)
						return false;
					float inv_det = 1.0 / det;

					/* calculaet distance from he->origin to ray origin */
					point3f tvec = p1 - he->origin->position;

					/* calculate U parameter and test bounds */
					u = dot(tvec, pvec) * inv_det;
					if (u < 0.0 || u > 1.0)
						return false;

					/* prepare for test V parameter */
					point3f qvec = cross(tvec, edge1);

					/* calculate V parameter and test bounds */
					v = dot(dir, qvec) * inv_det;
					if (v < 0.0 || u + v > 1.0)
						return false;

					/* calculate t, ray intersects triangle */
					t = dot(edge2, qvec) * inv_det;

				#endif

				param_t = t;

				/* check if negative t is allowed */
				if (!neg_t_allowed && t < 0.0)
					return false;

				/* check for segment test */
				if ( segment && (t < 0.0 || t > 1.0) )
					return false;

				point3f result = p1 + t * dir;

				intersection.x() = result.x();
				intersection.y() = result.y();
				intersection.z() = result.z();
				intersection.w() = result.w();

				return true;
			}

			// tests if the ray intersects the plane in which f lies.
			// if so, intersection-parameter is filled with coordinates
			// of this point.
			bool intersect(	const SnowSim::polygonal_mesh::face *f, const point3f& p1, const point3f& p2,
				bool segment, bool neg_t_allowed, point3f& intersection)
			{
				if (f->valence != 3)
					throw Exceptions::NotImplementedException("intersect for polygon");

				double t, u, v;

				point3f dir = p2 - p1;

				SnowSim::polygonal_mesh::halfedge* he = f->m_halfedge;

				/* find vectors for two edges sharing he->origin */
				point3f edge1 = he->next->origin->position - he->origin->position;
				point3f edge2 = he->next->next->origin->position - he->origin->position;

				/* begin calculating determinant - also used to calculate U parameter */
				point3f pvec = cross(dir, edge2);

				/* if determinant is near zero, ray lies in plane of triangle */
				double det = dot(edge1, pvec);

				#ifdef TEST_CULL /* define TEST_CULL if culling is desired */

					if (det < NearlyZero)
						return false;

					/* calculate distance from vert0 to ray origin */
					point3f tvec = p1 - he->origin->position;

					/* calculate U parameter and test bounds */
					u = dot(tvec, pvec);
					if (u < 0.0 || u > det)
						return false;

					/* prepare to test V parameter */
					point3f qvec = cross(tvec, edge1);

					/* calculate V parameter and the bounds */
					v = dot(dir, qvec);
					if (v < 0.0 || u + v > det)
						return false;

					/* calculate t, scale parameters, ray intersection triangle */
					t = dot(edge2, qvec);
					double inv_det = 1.0 / det;
					t *= inv_det;
					u *= inv_det;
					v *= inv_det;

				#else /* the non-culling branch */

					if (det > -NearlyZero && det < NearlyZero)
						return false;
					float inv_det = 1.0 / det;

					/* calculaet distance from he->origin to ray origin */
					point3f tvec = p1 - he->origin->position;

					/* calculate U parameter and test bounds */
					u = dot(tvec, pvec) * inv_det;
					if (u < 0.0 || u > 1.0)
						return false;

					/* prepare for test V parameter */
					point3f qvec = cross(tvec, edge1);

					/* calculate V parameter and test bounds */
					v = dot(dir, qvec) * inv_det;
					if (v < 0.0 || u + v > 1.0)
						return false;

					/* calculate t, ray intersects triangle */
					t = dot(edge2, qvec) * inv_det;

				#endif

				/* check if negative t is allowed */
				if (!neg_t_allowed && t < 0.0)
					return false;

				/* check for segment test */
				if ( segment && (t < 0.0 || t > 1.0) )
					return false;

				point3f result = p1 + t * dir;

				intersection.x() = result.x();
				intersection.y() = result.y();
				intersection.z() = result.z();
				intersection.w() = result.w();

				return true;
			}
		}
	}
}
