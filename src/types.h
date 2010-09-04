#pragma once
#include <cgv/math/linalg/linalg.h>
#include <cgv/scene/geometry/geometry.h>
//#include "MeshRepresentation/Voronoi/geom2d.h"
#include <cgv/scene/geometry/ray.h>
#include <cgv/scene/geometry/direction.h>

typedef float	PrecisionType;

const PrecisionType NearlyZero = 0.00001F;

#define SOLUTION
#define PI 3.14159265358979323846f
#define PI_2 1.57079632679489661923f

using namespace cgv::scene::geometry;
using namespace cgv::math::linalg;

/// 2D point
typedef point<float,2> point2f;
/// 3D point
typedef point<float,3> point3f;
/// 4D point
typedef point<float,4> point4f;

/// 3D ray
typedef ray<float,3> ray3f;
// 3D direction
typedef direction<float, 3> dir3f;

/// 4 by 4 Matrix
typedef fix_sqr_mat<float,4> Mat44f;

/// 4D Vector
typedef fix_col_vec<float,4> Vec4f;
/// 3D Vector
typedef fix_col_vec<float,3> Vec3f;

// an axis aligned bounding box
typedef axis_aligned_box<float, 3> BBox3f;

typedef sqr_mat<float> matf;
/// vertex drag modes
enum Vertex_Drag_Mode {NO=0,X=1,Y=2,XY=3,Z=4,XZ=5,YZ=6};
/// interaction modes
enum Interaction_Mode {VIEWPOINT,MARK,TRANSLATE,SPLIT,INSERT};
/// white color
const point3f white_color = point3f(1.0f,1.0f,1.0f);
/// black color
const point3f black_color = point3f(0.0f,0.0f,0.0f);
/// green color
const point3f green_color = point3f(0.0f,1.0f,0.0f);
/// darker green color
const point3f d_green_color = point3f(0.0f,0.85f,0.0f);
/// blue color
const point3f blue_color = point3f(0.0f,0.0f,1.0f);
/// violet color
const point3f violet_color = point3f(1.0f,0.0f,1.0f);
/// red color
const point3f red_color = point3f(1.0f,0.0f,0.0f);
/// yellow color
const point3f yellow_color = point3f(1.0f,1.0f,0.5f);
/// gray color
const point3f gray_color = point3f(0.5f,0.5f,0.5f);
/// ground normal
const point3f ground_normal = point3f(0.0f, 1.0f, 0.0f);

// top direction
const dir3f top_direction = dir3f(0.0f, 1.0f, 0.0f);
// down direction
const dir3f down_direction = dir3f(0.0f, -1.0f, 0.0f);

template<class T>
struct triple
{
	T first, second, third;

	triple(T a, T b, T c)
		: first(a), second(b), third(c)
	{}

	triple(T all)
		: first(all), second(all), third(all)
	{}
};
