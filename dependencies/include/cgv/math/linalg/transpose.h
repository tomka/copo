#pragma once
#include <cgv/math/linalg/col_vec.h>
#include <cgv/math/linalg/row_vec.h>
#include <cgv/math/linalg/mat.h>
#include <cgv/math/linalg/sqr_mat.h>
#include <cgv/math/linalg/diag_mat.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <class T>
col_vec<T> transpose(const row_vec<T>& v)
{
	return col_vec<T>(&v(0),v.size());
}

template <class T>
row_vec<T> transpose(const col_vec<T>& v)
{
	return row_vec<T>(&v(0),v.size());
}

template <class T>
mat<T> transpose(const mat<T>& m)
{
	mat<T> mt(m.ncols(),m.nrows());

	for(int i = 0; i < m.nrows(); i++)
	{
		for(int j = 0; j < m.ncols();j++)
			mt(j,i)=m(i,j);
		
	}
	return mt;
}

template <class T>
sqr_mat<T> transpose(const sqr_mat<T>& m)
{
	sqr_mat<T> mt(m.ncols());

	for(int i = 0; i < m.nrows(); i++)
	{
		for(int j = 0; j < m.ncols();j++)
			mt(j,i)=m(i,j);
		
	}
	return mt;
}


template <class T>
diag_mat<T> transpose(const diag_mat<T>& m)
{
	return m;
}

template<class T>
sym_mat<T> transpose(const sym_mat<T>& m)
{
	return m;
}

template<class T>
up_tri_mat<T> transpose(const lo_tri_mat<T>& m)
{
	up_tri_mat<T> um(&(m(0,0)),m.nrows());
	return um;
}

template<class T>
lo_tri_mat<T> transpose(const up_tri_mat<T>& m)
{
	lo_tri_mat<T> lm(&(m(0,0)),m.nrows());
	return lm;
}

		}
	}
}

