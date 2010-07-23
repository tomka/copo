#pragma once
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>

namespace cgv {
	namespace scene {
		namespace geometry {

/** A mathematical point with elementype T and dimension N. For vector and matrix computations it is possible to cast 
    the point into a row_vec (row vector) or a (column vector) to access the point as a position vector and it is also possible
	to assign row_vec's and col_vec's. The distinction between vectors, 
	normals, points and so on is made to handle differences between transformations of such
	mathematical primitives carefully.*/

template<class T,int N>
class point :public cgv::math::linalg::fix_col_vec<T,N>
{

public:

	///dimension of point
	static const int dim = N;

	///contructs a point with dimension N
	point(): cgv::math::linalg::fix_col_vec<T,N>(){}

	///copy constructor
	point(const point<T,N> &p):cgv::math::linalg::fix_col_vec<T,N>((cgv::math::linalg::fix_col_vec<T,N>)p)
	{
		
	}

	///
	point(const T&a): cgv::math::linalg::fix_col_vec<T,N>(a){}

	///constructs a point by a given position vector
	point(const cgv::math::linalg::fix_col_vec<T,N> &v):cgv::math::linalg::fix_col_vec<T,N>(v)
	{
		
	}

	///creates a point and initializes the first two components to x and y, Be careful, no dimension check is done!
	point(const T x, const T y) : cgv::math::linalg::fix_col_vec<T,N>()
	{
		this->x()=x;
		this->y()=y;
	}

	///creates a point and initializes the first three components with x,y and z. Be careful, no dimension check is done! 
	point(const T x, const T y, const T z) : cgv::math::linalg::fix_col_vec<T,N>()
	{
		this->x()=x;
		this->y()=y;
		this->z()=z;
	}

	///creates a point and initializes the first four components with x,y,z and w, Be  careful, no dimension check is done!
	point(const T x, const T y, const T z, const T w) : cgv::math::linalg::fix_col_vec<T,N>()
	{
		this->x()=x;
		this->y()=y;
		this->z()=z;
		this->w()=w;
	}

	///assigns a given point 
	point<T,N>& operator=(const point<T,N> &p)
	{
		for(int i=0; i < N; i++) 
			this->v[i]=p(i);
		
		return *this;
	}

	

	///assigns a given row vector
	point<T,N>& operator=(const cgv::math::linalg::fix_row_vec<T,N> &v)
	{
		for(int i=0; i < N; i++) 
			this->v[i]=v(i);
		return *this;
	}
	
	bool operator==(const point<T,N> &p) const
	{
		for(int i=0; i < N; i++) 
		{
			if(this->v[i]!=p(i))
				return false;
		}
		return true;
	}

	bool operator!=(const point<T,N> &p) const
	{
		
		return !(*this == p);
	}
	
	
		
	///cast into row_vector
	operator cgv::math::linalg::fix_row_vec<T,N>()
	{
		cgv::math::linalg::fix_row_vec<T,N> vv(this->v);
		return vv;
	}

	///cast into const row vector
	operator const cgv::math::linalg::fix_row_vec<T,N>() const 
	{
		cgv::math::linalg::fix_row_vec<T,N> vv(this->v);
		return vv;
	}
	
};

		}
	}
}

