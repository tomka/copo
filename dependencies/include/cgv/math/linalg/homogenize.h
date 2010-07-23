#pragma once
#include <cmath>
#include <cgv/math/linalg/col_vec.h>
#include <cgv/math/linalg/row_vec.h>
#include <cgv/math/linalg/mat.h>
#include <cgv/math/linalg/diag_mat.h>
#include <cgv/math/linalg/sqr_mat.h>


namespace cgv
{
	namespace math
	{
		namespace linalg
		{

/// returns a homogenized version of the row vector v  e.g. (x,y,z)^T -> (x,y,z,1)^T;
template<class T>
row_vec<T> homogenized(const row_vec<T>& v)
{
	row_vec<T> r(v.size()+1);
	for(int i = 0; i < v.size();i++)
		r(i)=v(i);
	r(v.size())=1;
	return r;
}

// returns a homogenized version of the column vector v e.g. (x,y,z) -> (x,y,z,1);
template<class T>
col_vec<T> homogenized(const col_vec<T>& v)
{
	col_vec<T> r(v.size()+1);
	for(int i = 0; i < v.size();i++)
		r(i)=v(i);
	r(v.size())=1;
	return r;
}

//returns a homogenized version of the matrix m e.g. ((a,b),(c,d)) -> ((a,b,0),(c,d,0),(0,0,1));
template<class T>
mat<T> homogenized(const mat<T>& m)
{
	mat<T> r(m.nrows()+1,m.ncols()+1);
	for(int i = 0; i < m.nrows();i++)
	{
		for(int j = 0; j < m.ncols();j++)
		{
			r(i,j) = m(i,j);
		}
		r(m.nrows(),i)=0;
		r(i,m.ncols())=0;
	}
	r(m.nrows(),m.ncols())=1;
	

	
	return r;
}

/// returns a homogenized version of the diagonal matrix m e.g.  ((a,0),(0,b)) -> ((a,0,0),(0,b,0),(0,0,1));
template<class T>
diag_mat<T> homogenized(const diag_mat<T>& m)
{
	diag_mat<T> r(m.nrows()+1);
	
	r(m.nrows())=1;
	return r;
}

/// returns a homogenized version of the square matrix m e.g. ((a,0),(0,b)) -> ((a,0,0),(0,b,0),(0,0,1));
template<class T>
sqr_mat<T> homogenized(const sqr_mat<T>& m)
{
	sqr_mat<T> r(m.nrows()+1);
	for(int i = 0; i < m.nrows();i++)
	{
		for(int j = 0; j < m.ncols();j++)
		{
			r(i,j) = m(i,j);
		}
		r(m.nrows(),i)=0;
		r(i,m.ncols())=0;
	}
	r(m.nrows(),m.ncols())=1;
	
	return r;
}



		}
	}
}

