#pragma once
#include <cmath>
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_diag_mat.h>
#include <cgv/math/linalg/fix_sqr_mat.h>


namespace cgv
{
	namespace math
	{
		namespace linalg
		{

/// returns a homogenized version of the row vector v  e.g. (x,y,z)^T -> (x,y,z,1)^T;

template <typename T, cgv::type::uint32 N>
fix_row_vec<T,N+1> homogenized(const fix_row_vec<T,N>& v)
{
	fix_row_vec<T,N+1> r;
	for(int i = 0; i < v.size();i++)
		r(i)=v(i);
	r(v.size())=1;
	return r;
}

// returns a homogenized version of the column vector v e.g. (x,y,z) -> (x,y,z,1);

template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N+1> homogenized(const fix_col_vec<T,N>& v)
{
	fix_col_vec<T,N+1> r;
	for(int i = 0; i < v.size();i++)
		r(i)=v(i);
	r(v.size())=1;
	return r;
}

//returns a homogenized version of the matrix m e.g. ((a,b),(c,d)) -> ((a,b,0),(c,d,0),(0,0,1));

template <typename T, cgv::type::uint32 N>
fix_mat<T,N+1,N+1> homogenized(const fix_mat<T,N,N>& m)
{
	fix_mat<T,N+1,N+1> r;
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

template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N+1> homogenized(const fix_diag_mat<T,N>& m)
{
	fix_diag_mat<T,N+1> r;
	
	r(m.nrows())=1;
	return r;
}

/// returns a homogenized version of the square matrix m e.g. ((a,0),(0,b)) -> ((a,0,0),(0,b,0),(0,0,1));

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N+1> homogenized(const fix_sqr_mat<T,N>& m)
{
	fix_sqr_mat<T,N+1> r;
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

