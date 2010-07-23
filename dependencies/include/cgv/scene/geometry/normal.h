#pragma once
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_normalize.h>

namespace cgv {
	namespace scene {
		namespace geometry {


/** A mathematical normal with element type T and dimension N. For vector and matrix calculation it is possible to cast 
    the normal into a row_vec (row vector) or a (column vector) to access a vectorial representation and it is also possible
	to assign row_vec's and col_vec's to a normal. The distinction between vectors, 
	normals, points and so on is made to handle differences between transformations of such
	mathematical primitives carefully.*/
template<class T,int N>
class normal : public cgv::math::linalg::fix_col_vec<T,N>
{

public:

	normal():cgv::math::linalg::fix_col_vec<T,N>(){}
	
	normal(const normal<T,N> &n) : cgv::math::linalg::fix_col_vec<T,N>((cgv::math::linalg::fix_col_vec<T,N>)n)
	{
		
	}

	normal(const cgv::math::linalg::fix_col_vec<T,N> &v) : cgv::math::linalg::fix_col_vec<T,N>(normalize(v))
	{
		
	}

	normal(const T& a) :  cgv::math::linalg::fix_col_vec<T,N>(a){}
	

	normal(const T x, const T y) : cgv::math::linalg::fix_col_vec<T,N>()
	{
		
		this->x()=x;
		this->y()=y;
		*this=cgv::math::linalg::normalize(this->vec);
		
	}

	normal(const T x, const T y, const T z) : cgv::math::linalg::fix_col_vec<T,N>()
	{
		this->x()=x;
		this->y()=y;
		this->z()=z;
		*this=cgv::math::linalg::normalize(*this);
	}

	normal(const T x, const T y, const T z, const T w) : cgv::math::linalg::fix_col_vec<T,N>()
	{
		this->x()=x;
		this->y()=y;
		this->z()=z;
		this->w()=w;
		*this=cgv::math::linalg::normalize(*this);
	}
	normal<T,N>& operator=(const normal<T,N> &n)
	{
		for(int i=0; i < N; i++) 
			this->v[i]=n(i);
		return *this;
	}

	

	normal<T,N>& operator=(const cgv::math::linalg::fix_row_vec<T,N> &vv)
	{
		for(int i=0; i < N; i++) 
			this->v[i]=vv(i);
		return *this;
	}


	static const int dim = N;

		
	


	operator const cgv::math::linalg::fix_row_vec<T,N>() const 
	{
		cgv::math::linalg::fix_row_vec<T,N> vv(this->v);
		return vv;
	}

	
};

		}
	}
}
