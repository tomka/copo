#include "utilities.h"
#include "Exceptions/NotImplementedException.h"

bool point_in_triangle(const point3f &p0,const point3f &p1,const point3f &p2, const point3f &p)	{
	point3f normal = cross(normalize(p1-p0),normalize(p2-p1));

	point3f n1 = cross(normalize(p-p1),normalize(p0-p));
	if(dot(normal,n1) < -0.001f) { // line p0 - p1 is offending
		return false;
	}

	point3f n2 = cross(normalize(p-p2),normalize(p1-p));
	if(dot(normal,n2) < -0.001f) { // line p1 - p2 is offending
		return false;
	}

	point3f n3 = cross(normalize(p-p0),normalize(p2-p));
	if(dot(normal,n3) < -0.001f) { // line p0 - p2 is offending
		return false;
	}

	return true;
}

bool point_in_triangle(const point3f &p0,const point3f &p1,const point3f &p2, const point3f &p, int& cw_index)	{
	point3f normal = cross(normalize(p1-p0),normalize(p2-p1));
	cw_index = -1;

	point3f n1 = cross(normalize(p-p1),normalize(p0-p));
	if(dot(normal,n1) < -0.001f) { // line p0 - p1 is offending
		cw_index = 0;
		return false;
	}

	point3f n2 = cross(normalize(p-p2),normalize(p1-p));
	if(dot(normal,n2) < -0.001f) { // line p1 - p2 is offending
		cw_index = 1;
		return false;
	}

	point3f n3 = cross(normalize(p-p0),normalize(p2-p));
	if(dot(normal,n3) < -0.001f) { // line p0 - p2 is offending
		cw_index = 2;
		return false;
	}

	return true;
}

/// check whether p is inside triangle p0,p1,p2 assuming p is on the plane of the triangle
/// is only checked against x and z
bool point_in_triangle_2d(const std::vector<point3f> &vertices, const point3f &p)
{
	if ( isLeft_right_handed_2d(vertices[0], vertices[1], p) < 0.0f )
		return false;
	if ( isLeft_right_handed_2d(vertices[1], vertices[2], p) < 0.0f )
		return false;
	if ( isLeft_right_handed_2d(vertices[2], vertices[0], p) < 0.0f)
		return false;

	return true;
}

// 2D line intersection
// first and last coordinate (x and z) are used for this
bool segment_segment_intersect(const point3f &p0,const point3f &p1,const point3f &p2, const point3f &p3, point3f &intersection)
{
	// WARNING: Works only in left handed sysem!
	float d=(p1[0]-p0[0])*(p3[2]-p2[2])-(p1[2]-p0[2])*(p3[0]-p2[0]);
	if (abs(d)<0.001) {return false;}
	float AB=((p0[2]-p2[2])*(p3[0]-p2[0])-(p0[0]-p2[0])*(p3[2]-p2[2]))/d;
	if (AB>0.0 && AB<1.0)
	{
		float CD=((p0[2]-p2[2])*(p1[0]-p0[0])-(p0[0]-p2[0])*(p1[2]-p0[2]))/d;
		if (CD>0.0 && CD<1.0)
        {
			intersection[0]=p0[0]+AB*(p1[0]-p0[0]);
			intersection[2]=p0[2]+AB*(p1[2]-p0[2]);
			return true;
        }
    }
	return false;

}

/// intersect line through p0p1 with ray starting at p2 through p3
/// if neg_t_allowed is true, the ray can also intersect "backwards"
/// see: http://www.faqs.org/faqs/graphics/algorithms-faq/ (1.03)
bool ray_line_intersect(const point3f &p0,const point3f &p1,const point3f &p2, const point3f &p3, bool neg_t_allowed, point3f &intersection)
{
	// WARNING: Works only in left handed sysem!
	float d=(p1[0]-p0[0])*(p3[2]-p2[2])-(p1[2]-p0[2])*(p3[0]-p2[0]);
	// if the line is parallel, return false
	if (abs(d)<0.001) {return false;}

	float dd = 1.0 / d;
	float r=((p0[2]-p2[2])*(p3[0]-p2[0])-(p0[0]-p2[0])*(p3[2]-p2[2]))*dd;
	float s=((p0[2]-p2[2])*(p1[0]-p0[0])-(p0[0]-p2[0])*(p1[2]-p0[2]))*dd;
	intersection[0]=p0[0]+r*(p1[0]-p0[0]);
	intersection[2]=p0[2]+r*(p1[2]-p0[2]);

	if ( ( !neg_t_allowed ) && ( s < 0.0f ) )
		return false;

	return true;
}

/// intersect segment through p0p1 with ray starting at p2 through p3
/// if neg_t_allowed is true, the ray can also intersect "backwards"
bool segment_ray_intersect(const point3f &p0,const point3f &p1,const point3f &p2, const point3f &p3, bool neg_t_allowed, point3f &intersection)
{
	// WARNING: Works only in left handed sysem!
	float d=(p1[0]-p0[0])*(p3[2]-p2[2])-(p1[2]-p0[2])*(p3[0]-p2[0]);
	// if the line is parallel, return false
	if (abs(d)<0.001) {return false;}

	float dd = 1.0 / d;
	float r=((p0[2]-p2[2])*(p3[0]-p2[0])-(p0[0]-p2[0])*(p3[2]-p2[2]))*dd;
	float s=((p0[2]-p2[2])*(p1[0]-p0[0])-(p0[0]-p2[0])*(p1[2]-p0[2]))*dd;
	intersection[0]=p0[0]+r*(p1[0]-p0[0]);
	intersection[2]=p0[2]+r*(p1[2]-p0[2]);

	if ( r < -0.0001f || r > 1.0001f)
		return false;

	if ( ( !neg_t_allowed ) && ( s < 0.0f ) )
		return false;

	return true;
}

/// intersects a ray with a polygon. False means that the ray hits non of the polygon segments
/// Are more than one hit available, the nearest is taken
/// The result is saved in "q"
bool intersect_ray_with_polygon(const std::vector<point3f> &vertices,  point3f &p, point3f &q, int &side)
{
	point3f new_point = q;
	int num_vertices = vertices.size();
	PrecisionType min_dist = -1.0;
	point3f hit;

	// go through all edges
	for (int i = 0; i < num_vertices; ++i)
	{
		int index2 = (i+1) % num_vertices;

		if ( segment_ray_intersect(vertices[i], vertices[index2], p, q, true, hit) )
		{
			PrecisionType l = length(p-hit);
			if (l < min_dist || min_dist < 0.0)
			{
				min_dist = l;
				new_point = hit;
				side = index2;
			}
		}
	}

	if ( min_dist < 0.0 )
		return false;

	q = new_point;

	return true;
}

/// A 2d XZ plane function to project a ray onto a bounding box.
/// If the ray starting at p0 and going through p2 does not hit the box,
/// return false. Otherwise true.
bool project_ray_to_bb_XZ(const BBox3f, const point3f &p0, const point3f &p2)
{


	return false;
}

bool clip_line_to_triangle(const std::vector<point3f> &vertices, int num_vertices,
						   point3f &p, point3f &q)
{
	bool p_outside;
	bool q_outside;
	int index2;

	// go through all edges
	for (int i = 0; i < num_vertices; ++i) {

		index2 = (i+1) % num_vertices;

		p_outside = ( isLeft_right_handed_2d(vertices[i], vertices[index2], p) < 0.0 );
		q_outside = ( isLeft_right_handed_2d(vertices[i], vertices[index2], q) < 0.0 );

		if (p_outside) {
			if (q_outside) {
				// if both points are "outside" of the edge, we can leave
				return false;
			} else {
				segment_segment_intersect(vertices[i], vertices[index2], p, q, p); // save the intersection in p
			}
		} else {
			if (q_outside) {
				segment_segment_intersect(vertices[i], vertices[index2], p, q, q); // save the intersection in q
			}
		}
	}

	return true;
}

/// clip line pq to triangle. False means that the line is not in the triangle and does not cross it
bool clip_line_to_triangle(const std::vector<point3f> &vertices, int num_vertices,
		point3f &p, point3f &q, int &side_hit)
{
	bool p_outside;
	bool q_outside;
	int index2;
	side_hit = -1;

	// go through all edges
	for (int i = 0; i < num_vertices; ++i) {

		index2 = (i+1) % num_vertices;

		p_outside = ( isLeft_right_handed_2d(vertices[i], vertices[index2], p) < 0.0 );
		q_outside = ( isLeft_right_handed_2d(vertices[i], vertices[index2], q) < 0.0 );

		if (p_outside) {
			if (q_outside) {
				// if both points are "outside" of the edge, we can leave
				return false;
			} else {
				side_hit = index2;
				segment_segment_intersect(vertices[i], vertices[index2], p, q, p); // save the intersection in p
			}
		} else {
			if (q_outside) {
				side_hit = index2;
				segment_segment_intersect(vertices[i], vertices[index2], p, q, q); // save the intersection in q
			}
		}
	}

	return true;
}

/// clip a ray to a triangle. False means that the ray starts not in the triangle and does not cross it
bool clip_ray_to_polygon(const std::vector<point3f> &vertices,  point3f &p, point3f &q)
{
	int num_vertices = vertices.size();
	// go through all edges
	for (int i = 0; i < num_vertices; ++i) {

		int index2 = (i+1) % num_vertices;
		bool p_inside =  !( isLeft_right_handed_2d(vertices[i], vertices[index2], p) < 0.0 );

		if ( p_inside )
		{
			point3f hit;
			bool q_crosses = segment_ray_intersect( vertices[i], vertices[index2], p, q, false, hit);
			if ( q_crosses ) {
				q = hit;
				return true;
			}
		}
	}

	return false;
}

/// clip a ray to a triangle. False means that the ray starts not in the triangle and does not cross it
bool clip_ray_to_polygon(const std::vector<point3f> &vertices,  point3f &p, point3f &q, int &side)
{
	int num_vertices = vertices.size();
	// go through all edges
	for (int i = 0; i < num_vertices; ++i) {

		int index2 = (i+1) % num_vertices;
		bool p_inside =  !( isLeft_right_handed_2d(vertices[i], vertices[index2], p) < 0.0 );

		if ( p_inside )
		{
			point3f hit;
			bool q_crosses = segment_ray_intersect( vertices[i], vertices[index2], p, q, false, hit);
			if ( q_crosses ) {
				q = hit;
				side = index2;
				return true;
			}
		}
	}

	side = -1;
	return false;
}

/// clip polygon to triangle. False means that the polygon is not in the triangle and does not cross it
bool clip_polygon_to_triangle(const std::vector<point3f> &tri_vertices, std::vector<point3f> &polygon)
{
	throw SnowSim::Exceptions::NotImplementedException("clip_polygon_to_triangle is not implemented!");

	int index2;
	int tri_num_v = tri_vertices.size();
	int polygon_num_v = polygon.size();

	std::vector<point3f> clipped_polygon;

	// go through all edges of the "clipping window"
	for (int i = 0; i < tri_num_v; ++i)
	{
		index2 = (i+1) % tri_num_v;

		bool last_was_outside = false;

		// check every line of the polygon
		std::vector<point3f>::iterator p_it = polygon.begin();
		int p = 0;
		for (; p_it != polygon.end(); ++p_it)
		{
			++p;
			int p_index2 = (p+1) % polygon.size();

			bool p_outside = ( isLeft_right_handed_2d(tri_vertices[i], tri_vertices[index2], polygon[p]) < -0.0001f );
			bool q_outside = ( isLeft_right_handed_2d(tri_vertices[i], tri_vertices[index2], polygon[p_index2]) < -0.0001f );

			// We now know which part (if any) is outside of the current triangle side.
			// A polygon being inside of the triangle has all points in it, so continue with
			// next side if both points are in

			if (p_outside) {
				if (q_outside) {
					// both points are outside - no new points, remove instead the two
					if ( last_was_outside )
					{
						// if the last segment was also outside, remove the point in between
						p_it = polygon.erase( p_it ); // position is now the next element after removed vertex
						--p_it; // go one step back, to make the for loop work as expected
					} else
						last_was_outside = true;

				} else {
					// p outside, q inside
					last_was_outside = false;
					//line_line_intersect(vertices[i], vertices[index2], p, q, p); // save the intersection in p
					//clipped_polygon.push_back()
					return false; // temp
				}
			} else {
				if (q_outside) {
					last_was_outside = false;
					// p inside, q outside
					return false; // temp
				} else {
					last_was_outside = false;
					// both points inside or on the clipping window - we don't need to change the point set
					// just go on
				}
			}
		}
	}

	return true;
}

inline unsigned int countdigits(unsigned int x)
{
    unsigned count=1;
    unsigned int value= 10;
    while (x>=value)
    {
        value*=10;
        count++;
    }
    return count;
}

/// transform a 3D vector by a 44 Matrix
point3f operator*(const fix_sqr_mat<float,4>& m, const fix_col_vec<float,3>& v3)
{
	fix_col_vec<float,4> r(float(0));
	fix_col_vec<float,4> v;
	v.set(v3.x(), v3.y(), v3.z(),1.0f);


	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4;j++)
			r(i) += m(i,j)*v(j);

	}
	fix_col_vec<float,3> ret;
	ret.set(r.x() / r.w(), r.y() / r.w(), r.z() / r.w());
	return ret;
}

/// compute the area of a triangle
float area2D_triangle(const point3f &p0,const point3f &p1,const point3f &p2) {
	return isLeft_left_handed_2d( p0, p1, 2 ) * 0.5f;
}

// area3D_Polygon(): computes the area of a 3D planar polygon
//    Input:  int n = the number of vertices in the polygon
//            Point* V = an array of n+2 vertices in a plane
//                       with V[n]=V[0] and V[n+1]=V[1]
//            Point N = unit normal vector of the polygon's plane
//    Return: the (float) area of the polygon
float
area3D_Polygon( int n, point3f* V, point3f N )
{
    float area = 0;
    float an, ax, ay, az;  // abs value of normal and its coords
    int   coord;           // coord to ignore: 1=x, 2=y, 3=z
    int   i, j, k;         // loop indices

    // select largest abs coordinate to ignore for projection
    ax = (N.x()>0 ? N.x() : -N.x());     // abs x-coord
    ay = (N.y()>0 ? N.y() : -N.y());     // abs y-coord
    az = (N.z()>0 ? N.z() : -N.z());     // abs z-coord

    coord = 3;                     // ignore z-coord
    if (ax > ay) {
        if (ax > az) coord = 1;    // ignore x-coord
    }
    else if (ay > az) coord = 2;   // ignore y-coord

    // compute area of the 2D projection
    for (i=1, j=2, k=0; i<=n; i++, j++, k++)
        switch (coord) {
        case 1:
            area += (V[i].y() * (V[j].z() - V[k].z()));
            continue;
        case 2:
            area += (V[i].x() * (V[j].z() - V[k].z()));
            continue;
        case 3:
            area += (V[i].x() * (V[j].y() - V[k].y()));
            continue;
        }

    // scale to get area before projection
    an = sqrt( ax*ax + ay*ay + az*az);  // length of normal vector
    switch (coord) {
    case 1:
        area *= (an / (2*ax));
        break;
    case 2:
        area *= (an / (2*ay));
        break;
    case 3:
        area *= (an / (2*az));
    }
    return area;
}
