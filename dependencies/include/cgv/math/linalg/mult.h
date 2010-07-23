#pragma once

#include <cgv/math/linalg/col_vec.h>
#include <cgv/math/linalg/row_vec.h>
#include <cgv/math/linalg/mat.h>
#include <cgv/math/linalg/diag_mat.h>
#include <cgv/math/linalg/sqr_mat.h>
#include <cgv/math/linalg/scl_mult.h>





namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <class T>
T operator*(const row_vec<T>& v1,const col_vec<T>& v2)
{
	T p = 0;

	for(int i = 0;i < v1.size();i++)
		p += v1(i) * v2(i);

	return p;
}

template <class T>
sqr_mat<T> operator*(const col_vec<T>& v1,const row_vec<T>& v2)
{
	sqr_mat<T> m(v1.size());

	for(int i = 0;i < v1.size();i++)
		for(int j = 0; j < v1.size();j++)
		{
			m(i,j)=v1(i)*v2(j);
		}
		

	return m;
}


template <class T>
row_vec<T> operator*(const row_vec<T>& v,const mat<T>& m)
{
		
	T x=0;
	row_vec<T> r(x,m.ncols());

	for(int c = 0;c < r.size();c++)
	{
		for(int i = 0;i < v.size();i++)
		{
			r(c) += v(i)*m(i,c); 
		}
	}

	return r;
}

template <class T>
row_vec<T> operator*(const row_vec<T>& v, const sqr_mat<T>& m)
{
	row_vec<T> r(0,m.ncols());

	for(int c = 0;c < r.size();c++)
	{
		for(int i = 0;i < v.size();i++)
		{
			r(c) += v(i)*m(i,c); 
		}
	}

	return r;
}


template <class T>
row_vec<T> operator*(const row_vec<T>& m1, const diag_mat<T>& m2)
{
		
	row_vec<T> r(0,m1.ncols());

	for(int c = 0;c < r.size();c++)
	{
		for(int i = 0;i < m2.size();i++)
		{
			r(c) += m2(i)*m1(i,c); 
		}
	}

	return r;
}

template <class T>
col_vec<T> operator*(const mat<T>& m, const col_vec<T>& v)
{
		
	col_vec<T> r(0.0,m.nrows());

	for(int i = 0; i < m.nrows(); i++)
	{
		for(int j = 0; j < m.ncols();j++)
			r(i) += m(i,j)*v(j);
		
	}

	return r;
}

template <class T>
col_vec<T> operator*(const sqr_mat<T>& m, const col_vec<T>& v)
{
		
	col_vec<T> r(0.0,m.nrows());

	for(int i = 0; i < m.nrows(); i++)
	{
		for(int j = 0; j < m.ncols();j++)
			r(i) += m(i,j)*v(j);
		
	}

	return r;
}

template <class T>
mat<T> operator*(const mat<T>& m1, const mat<T>& m2)
{
		
	int nrows=m1.nrows();
	int ncols=m2.ncols();
	

	mat<T> p(0.0,ncols,nrows);
	
	for(int i = 0; i < nrows; i++)
	{
		for(int j = 0; j < ncols;j++)
		{
			for(int k = 0; k < m1.ncols(); k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}
	

	return p;
}

template <class T>
mat<T> operator*(const mat<T>& m1, const sqr_mat<T>& m2)
{
		
	int nrows=m1.nrows();
	int ncols=m2.ncols();
	

	mat<T> p(0.0,ncols,nrows);
	
	for(int i = 0; i < nrows; i++)
	{
		for(int j = 0; j < ncols;j++)
		{
			for(int k = 0; k < m1.ncols(); k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}
	

	return p;
}

template <class T>
mat<T> operator*(const sqr_mat<T>& m1, const mat<T>& m2)
{
		
	int nrows=m1.nrows();
	int ncols=m2.ncols();
	

	mat<T> p(0.0,ncols,nrows);
	
	for(int i = 0; i < nrows; i++)
	{
		for(int j = 0; j < ncols;j++)
		{
			for(int k = 0; k < m1.ncols(); k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}
	

	return p;
}

template <class T>
sqr_mat<T> operator*(const sqr_mat<T>& m1, const sqr_mat<T>& m2)
{
		
	int nrows=m1.nrows();
	int ncols=m2.ncols();
	

	sqr_mat<T> p(0.0,ncols);
	
	for(int i = 0; i < nrows; i++)
	{
		for(int j = 0; j < ncols;j++)
		{
			for(int k = 0; k < m1.ncols(); k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}
	

	return p;
}

template <class T>
mat<T> operator*(const mat<T>& m1, const diag_mat<T>& m2)
{
		
	int nrows=m1.nrows();
	int ncols=m2.ncols();
	

	mat<T> p(0.0,ncols,nrows);
	
	for(int i = 0; i < nrows; i++)
	{
		for(int j = 0; j < ncols;j++)
		{
			
				p(i,j) += m1(i,j)*m2(j,j); 
		}
		
	}
	

	return p;
}




template <class T>
mat<T> operator*( const diag_mat<T>& m1,const mat<T>& m2)
{
		
	int nrows=m1.nrows();
	int ncols=m2.ncols();
	

	mat<T> p(0.0,ncols,nrows);
	
	for(int i = 0; i < nrows; i++)
	{
		for(int j = 0; j < ncols;j++)
		{
			
				p(i,j) += m2(i,j)*m1(j,j); 
		}
		
	}
	

	return p;
}



template <class T>
col_vec<T> operator*(const diag_mat<T>& m, const col_vec<T>& v)
{
		
	col_vec<T> r(0.0,m.nrows());

	for(int i = 0; i < m.nrows(); i++)
	{
		
			r(i) = m(i,i)*v(i);
		
	}

	return r;
}


		}
	}
}






