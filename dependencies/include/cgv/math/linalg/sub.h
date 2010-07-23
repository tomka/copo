#pragma once
#include <cgv/math/linalg/col_vec.h>
#include <cgv/math/linalg/row_vec.h>
#include <cgv/math/linalg/mat.h>
#include <cgv/math/linalg/diag_mat.h>
#include <cgv/math/linalg/scl_mult.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <class T>
row_vec<T> operator-=(row_vec<T>& v1,const row_vec<T>& v2)
{
	

	for(int i = 0;i < v1.size();i++)
		v1(i)-=  v2(i);

	return v1;
}

template <class T>
col_vec<T> operator-=(col_vec<T>& v1,const col_vec<T>& v2)
{
	

	for(int i = 0;i < v1.size();i++)
		v1(i)-=  v2(i);

	return v1;
}

template <class T>
row_vec<T> operator-(const row_vec<T>& v)
{
	T s = -1;
	return s*v;
}

template <class T>
col_vec<T> operator-(const col_vec<T>& v)
{
	T s = -1;
	return s*v;
}

template <class T>
mat<T> operator-(const mat<T>& v)
{
	T s = -1;
	return s*v;
}

template <class T>
sqr_mat<T> operator-(const sqr_mat<T>& v)
{
	T s = -1;
	return s*v;
}
template <class T>
diag_mat<T> operator-(const diag_mat<T>& v)
{
	T s = -1;
	return s*v;
}

template <class T>
row_vec<T> operator-(const row_vec<T>& v1,const row_vec<T>& v2)
{
	row_vec<T> diff(v1.size());

	for(int i = 0;i < v1.size();i++)
		diff(i)=v1(i) - v2(i);

	return diff;
}

template <class T>
col_vec<T> operator-(const col_vec<T>& v1,const col_vec<T>& v2)
{
	col_vec<T> diff(v1.size());

	for(int i = 0;i < v1.size();i++)
		diff(i)=v1(i)-v2(i);

	return diff;
}

template <class T>
mat<T> operator-(const mat<T>& m1,const mat<T>& m2)
{
	mat<T> diff(m1.nrows(),m1.ncols());

	for(int i = 0; i < m1.nrows(); i++)
	{
		for(int j = 0; j < m1.ncols();j++)
		{
			diff(i,j)=m1(i,j)-m2(i,j);	
		}
	
	}
	
	return diff;
}

template <class T>
mat<T> operator-(const mat<T>& m1,const diag_mat<T>& m2)
{
	mat<T> diff(m1.nrows(),m1.ncols());

	for(int i = 0; i < m1.nrows(); i++)
	{
		for(int j = 0; j < m1.ncols();j++)
		{
			diff(i,j)=m1(i,j)-m2(i,j);	
		}
	
	}
	
	return diff;
}

template <class T>
mat<T> operator-(const diag_mat<T>& m1,const mat<T>& m2)
{
	mat<T> diff(m1.nrows(),m1.ncols());

	for(int i = 0; i < m1.nrows(); i++)
	{
		for(int j = 0; j < m1.ncols();j++)
		{
			diff(i,j)=m1(i,j)-m2(i,j);	
		}
	
	}
	
	return diff;
}

template <class T>
sqr_mat<T> operator-(const diag_mat<T>& m1,const sqr_mat<T>& m2)
{
	sqr_mat<T> diff(m1.nrows());

	for(int i = 0; i < m1.nrows(); i++)
	{
		for(int j = 0; j < m1.ncols();j++)
		{
			diff(i,j)=m1(i,j)-m2(i,j);	
		}
	
	}
	
	return diff;
}

template <class T>
diag_mat<T> operator-(const diag_mat<T>& m1,const diag_mat<T>& m2)
{
	diag_mat<T> diff(m1.nrows());

	for(int i = 0; i < m1.nrows(); i++)
	{
		
		diff(i)=m1(i)-m2(i);	
		
	
	}
	
	return diff;
}



		}
	}
}

