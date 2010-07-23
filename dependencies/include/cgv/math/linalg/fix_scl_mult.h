#pragma once
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_diag_mat.h>
#include <cgv/math/linalg/fix_sqr_mat.h>
#include <cgv/math/linalg/fix_quaternion.h>


namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <typename T>
fix_quaternion<T> operator*(const T s,const fix_quaternion<T> q)
{
	fix_quaternion<T> p;
	
	for(int i = 0;i < 4;i++)
		p(i) = q(i) * s;

	return p;
}

template <typename T>
fix_quaternion<T> operator*(const fix_quaternion<T> q,const T s)
{
	fix_quaternion<T> p;
	
	for(int i = 0;i < 4;i++)
		p(i) = q(i) * s;

	return p;
}

template <typename T, cgv::type::uint32 N>
fix_row_vec<T,N> operator*(const T s,const fix_row_vec<T,N> v)
{
	fix_row_vec<T,N> p;
	
	for(int i = 0;i < N;i++)
		p(i) = v(i) * s;

	return p;
}

template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> operator*(const T s,const fix_col_vec<T,N> v)
{
	fix_col_vec<T,N> p;
	
	for(int i = 0;i < N;i++)
		p(i) = v(i) * s;

	return p;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
fix_mat<T,N,M> operator*(const T s,const fix_mat<T,N,M> m)
{
	fix_mat<T,N,M> p;
	
	for(int i = 0;i < m.nrows();i++)
		for(int j= 0;j < m.ncols();j++)
			p(i,j) = m(i,j) * s;

	return p;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> operator*(const T s,const fix_sqr_mat<T,N> m)
{
	fix_sqr_mat<T,N> p;
	
	for(int i = 0;i < m.nrows();i++)
		for(int j= 0;j < m.ncols();j++)
			p(i,j) = m(i,j) * s;

	return p;
}

template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N> operator*(const T s,const fix_diag_mat<T,N> m)
{
	fix_diag_mat<T,N> p;
	
	for(int i = 0;i < N;i++)
				p(i) = m(i) * s;

	return p;
}


		}
	}
}

