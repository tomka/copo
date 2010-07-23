#include "epsilon.h"
#include "mesh_geometry.h"
#include <algorithm>
#include <math.h>
#include "utilities.h"

template class mesh_geometry<float>;
template class mesh_geometry<double>;

/// construct empty triangle mesh
template <typename C, class P>
mesh_geometry<C,P>::mesh_geometry()
{
}

template <typename C, class P>
typename mesh_geometry<C,P>::point_type mesh_geometry<C,P>::transform(const point_type& p, ShapeType shape)
{
	switch (shape) {
		case TRIANGLE :
			if (p.x()+p.y() > 0)
				return point_type(-p.x(),-p.y());
			else
				return p;
		case CIRCLE :
			{
				coord_type r = (coord_type)0.5*(p.x()+1);
				coord_type a = (coord_type)3.141592654*p.y();
				return point_type(r*cos(a),r*sin(a));
			}
	}
	return p;
}

/// generate random points
template <typename C, class P>
void mesh_geometry<C,P>::generate_sample_data_set(unsigned int n, ShapeType shape, SamplingType sampling, bool do_random_shuffle)
{
	unsigned int m;
	if (sampling == UNIFORM) {
		m = (unsigned int) sqrt((double)n);
		n = m*m;
	}
	unsigned int N = get_nr_vertices();
	P.resize(N+n);
	if (sampling == UNIFORM) {
		double scale = 2.0/m;
		for (unsigned int i=0; i<m; ++i) {
			for (unsigned int j=0; j<m; ++j) {
				point_type p((coord_type)(scale*i-1.0),(coord_type)(scale*j-1.0));
				p_of_vi(N+i*m+j) = transform(p,shape);
			}
		}
	}
	else {
		for (unsigned int i=0; i<n; ++i) {
			point_type p((coord_type)(sampling == RANDOM ? gen_rand() : gen_rand_high_res()),(coord_type)(sampling == RANDOM ? gen_rand() : gen_rand_high_res()));
			p_of_vi(N+i) = transform(p,shape);
		}
	}
	if (do_random_shuffle)
		std::random_shuffle(P.begin()+N, P.end());
}


/// remove all vertices and triangles
template <typename C, class P>
void mesh_geometry<C,P>::clear_geometry()
{
	P.clear();
}

template <typename C, class P>
void mesh_geometry<C,P>::add_point(const point_type& p)
{
	P.push_back(p);
}

/// check if point p is outside with respect to the line through p1 and p2
template <typename C, class P>
bool mesh_geometry<C,P>::is_outside(const point_type& p, const point_type& p1, const point_type& p2)
{
	const coord_type& x = p.x();
	const coord_type& y = p.y();
	const coord_type& x1 = p1.x();
	const coord_type& y1 = p1.y();
	const coord_type& x2 = p2.x();
	const coord_type& y2 = p2.y();
	coord_type lhs = (x1-x)*(y2-y);
	coord_type rhs = (y1-y)*(x2-x);
	coord_type squared_length = (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
	coord_type eps = epsilon<coord_type>::get_eps()*squared_length;
	return  lhs+eps < rhs;
}

/// check if point p is inside with respect to the line through p1 and p2
template <typename C, class P>
bool mesh_geometry<C,P>::is_inside(const point_type& p, const point_type& p1, const point_type& p2)
{
	return is_outside(p,p2,p1);
}
/// check if point p is incident to the segment from p1 to p2
template <typename C, class P>
bool mesh_geometry<C,P>::is_incident(const point_type& p, const point_type& p1, const point_type& p2)
{
	if (is_outside(p,p1,p2))
		return false;
	if (is_outside(p,p2,p1))
		return false;
	return is_between(p, p1, p2);
}

/// check if point p projects inside the segment from p1 to p2
template <typename C, class P>
bool mesh_geometry<C,P>::is_between(const point_type& p, const point_type& p1, const point_type& p2)
{
	const coord_type& x = p.x();
	const coord_type& y = p.y();
	const coord_type& x1 = p1.x();
	const coord_type& y1 = p1.y();
	const coord_type& x2 = p2.x();
	const coord_type& y2 = p2.y();
	coord_type squared_length = (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
	coord_type eps = epsilon<coord_type>::get_eps()*squared_length;
	return (x2-x1)*(x-x1)+(y2-y1)*(y-y1) > eps;
}

/// compute the squared distance between two points
template <typename C, class P>
C mesh_geometry<C,P>::sqr_dist(const point_type& p0, const point_type& p1)
{
	C dx = p0.x()-p1.x();
	C dy = p0.y()-p1.y();
	return dx*dx+dy*dy;
}

/// compute the circum center of a triangle
template <typename C, class P>
typename mesh_geometry<C,P>::point_type mesh_geometry<C,P>::compute_circum_center(const point_type& p0, const point_type& p1, const point_type& p2)
{
	const coord_type& x0 = p0.x();
	const coord_type& y0 = p0.y();
	const coord_type& x1 = p1.x();
	const coord_type& y1 = p1.y();
	const coord_type& x2 = p2.x();
	const coord_type& y2 = p2.y();
	coord_type inv_denom = (coord_type)0.5/((y2*x1+x0*y1+x2*y0)-(x1*y0+x0*y2+y1*x2));
	return point_type(inv_denom*((x0*x0*y1+y0*y0*y1+y1*y1*y2+x1*x1*y2+y0*y2*y2+y0*x2*x2)-
	                             (x0*x0*y2+y0*x1*x1+y1*y2*y2+y0*y0*y2+y0*y1*y1+y1*x2*x2)),
				         inv_denom*((x1*x2*x2+x0*x1*x1+x1*y2*y2+x2*x0*x0+x2*y0*y0+x0*y1*y1)-
							           (x1*x0*x0+x0*x2*x2+x1*y0*y0+x2*y1*y1+x0*y2*y2+x2*x1*x1)));
}

