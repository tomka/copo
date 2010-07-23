#pragma once

#include <set>
#include <cgv/scene/geometry/geometry.h>
#include <cgv/math/linalg/linalg.h>
#include <types.h>
#include <iterator>

// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
inline float isLeft_right_handed_2d(const point3f &p0, const point3f &p1, const point3f &p2) {
	return ( ( ( p1.z() - p0.z() ) * ( p2.x() - p0.x() ) )
			   - ( ( p1.x() - p0.x() ) * ( p2.z() - p0.z() ) ) );
}

// directs_to_left_2d(): tests if a second lines direction is relative Left|On|Right-handed to the first line
//    Input:  three points P0, P1 (line 1) and P2, P3 (line 2)
//    Return: >0 for line 2 is directed to the left with respect to the line 1
//            =0 for line 2 being on line 1 (either direction fore or back)
//            <0 for line 2 is directed to the right with respect to the line 1
//	  Remarks: Only X and Z variables are used
inline float directs_to_left_2d(const point3f &p0, const point3f &p1, const point3f &p2, const point3f &p3) {
	return ( ( ( p1.z() - p0.z() ) * ( p3.x() - p2.x() ) )
				   - ( ( p1.x() - p0.x() ) * ( p3.z() - p2.z() ) ) );
}

// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
inline float isLeft_left_handed_2d(const point3f &p0, const point3f &p1, const point3f &p2) {
	return isLeft_right_handed_2d(p0, p1, p2);
}

/// A 2d XZ plane function to project a ray onto a bounding box.
/// If the ray starting at p0 and going through p2 does not hit the box,
/// return false. Otherwise true.
bool project_ray_to_bb_XZ(const BBox3f, const point3f &p0, const point3f &p2);

/// check whether p is inside triangle p0,p1,p2 assuming p is on the plane of the triangle
/// is only checked against x and z
bool point_in_triangle_2d(const std::vector<point3f> &vertices, const point3f &p);

/// compute the area of a triangle
float area_of_triangle(const point3f &p0,const point3f &p1,const point3f &p2);


/// check whether p is inside triangle p0,p1,p2 assuming p is on the plane of the triangle
bool point_in_triangle(const point3f &p0,const point3f &p1,const point3f &p2, const point3f &p);

/// check whether p is inside triangle p0,p1,p2 assuming p is on the plane of the triangle and return the offending edge index (clockwise)
bool point_in_triangle(const point3f &p0,const point3f &p1,const point3f &p2, const point3f &p, int& cw_index);

/// clip line pq to triangle. False means that the line is not in the triangle and does not cross it
bool clip_line_to_triangle(const std::vector<point3f> &vertices, int num_vertices,
		point3f &p, point3f &q);

/// clip line pq to triangle. False means that the line is not in the triangle and does not cross it
bool clip_line_to_triangle(const std::vector<point3f> &vertices, int num_vertices,
		point3f &p, point3f &q, int &side_hit);

/// clip a ray to a triangle. False means that the ray starts not in the triangle
/// The result is saved in "q"
bool clip_ray_to_polygon(const std::vector<point3f> &vertices,  point3f &p, point3f &q);

/// clip a ray to a triangle. False means that the ray starts not in the triangle and does not cross it
/// The result is saved in "q"
/// In integer "side" is the hit side remembered. If a hit occured between v0 and v1, it will be 1 and so on
bool clip_ray_to_polygon(const std::vector<point3f> &vertices,  point3f &p, point3f &q, int &side);

/// intersects a ray with a polygon. False means that the ray hits non of the polygon segments
/// The result is saved in "q"
/// In integer "side" is the hit side remembered. If a hit occured between v0 and v1, it will be 1 and so on
bool intersect_ray_with_polygon(const std::vector<point3f> &vertices,  point3f &p, point3f &q, int &side);

/// clip polygon to triangle. False means that the polygon is not in the triangle and does not cross it
bool clip_polygon_to_triangle(const std::vector<point3f> &tri_vertices, std::vector<point3f> &polygon);

/// intersect line segment p0p1 with line p2p3
bool segment_segment_intersect(const point3f &p0,const point3f &p1,const point3f &p2, const point3f &p3, point3f &intersection);

/// intersect line through p0p1 with ray starting at p2 through p3
/// if neg_t_allowed is true, the ray can also intersect "backwards"
bool ray_line_intersect(const point3f &p0,const point3f &p1,const point3f &p2, const point3f &p3, bool neg_t_allowed, point3f &intersection);

/// intersect segment through p0p1 with ray starting at p2 through p3
/// if neg_t_allowed is true, the ray can also intersect "backwards"
bool segment_ray_intersect(const point3f &p0,const point3f &p1,const point3f &p2, const point3f &p3, bool neg_t_allowed, point3f &intersection);

inline unsigned int countdigits(unsigned int x);

/// transform a 3D vector by a 44 Matrix
point3f operator*(const fix_sqr_mat<float,4>& m, const fix_col_vec<float,3>& v3);

/// Template to subtract two Sets
template <class T>
void setsubtract(std::set<T>& viSet, const std::set<T>& vjSet)
{
	typename std::set<T>::const_iterator jter;

	for (jter = vjSet.begin(); jter != vjSet.end(); ++jter) {
		viSet.erase(*jter);
		if (viSet.empty())
			break;
	}
}

/// Template to intersect two Sets
template <typename T>
void setintersect(std::set<T>& viSet, const std::set<T>& vjSet)
{
	bool finished = false;
	typename std::set<T>::iterator iter;

	while (!finished)
	{
		finished = true;

		for (iter = viSet.begin(); iter != viSet.end(); ++iter) {
			if (vjSet.find(*iter) == vjSet.end()) {
				viSet.erase(*iter);
				finished = false;
				break;
			}
		}
	}
}
