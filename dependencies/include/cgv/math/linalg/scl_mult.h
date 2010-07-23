#pragma once
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

template <class T>
row_vec<T> operator*(const T s,const row_vec<T> v)
{
	row_vec<T> p(v.size());
	
	for(int i = 0;i < v.size();i++)
		p(i) = v(i) * s;

	return p;
}

template <class T>
col_vec<T> operator*(const T s,const col_vec<T> v)
{
	col_vec<T> p(v.size());
	
	for(int i = 0;i < v.size();i++)
		p(i) = v(i) * s;

	return p;
}

template <class T>
mat<T> operator*(const T s,const mat<T> m)
{
	mat<T> p(m.nrows(),m.ncols());
	
	for(int i = 0;i < m.nrows();i++)
		for(int j= 0;j < m.ncols();j++)
			p(i,j) = m(i,j) * s;

	return p;
}

template <class T>
sqr_mat<T> operator*(const T s,const sqr_mat<T> m)
{
	sqr_mat<T> p(m.nrows());
	
	for(int i = 0;i < m.nrows();i++)
		for(int j= 0;j < m.ncols();j++)
			p(i,j) = m(i,j) * s;

	return p;
}

template <class T>
diag_mat<T> operator*(const T s,const diag_mat<T> m)
{
	diag_mat<T> p(m.nrows());
	
	for(int i = 0;i < m.nrows();i++)
				p(i) = m(i) * s;

	return p;
}


		}
	}
}

