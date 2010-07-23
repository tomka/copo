#pragma once
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_diag_mat.h>
#include <cgv/math/linalg/fix_scl_mult.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <typename T, cgv::type::uint32 N>
fix_row_vec<T,N> operator-=(fix_row_vec<T,N>& v1,const fix_row_vec<T,N>& v2)
{
	

	for(int i = 0;i < N;i++)
		v1(i)-=  v2(i);

	return v1;
}

template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> operator-=(fix_col_vec<T,N>& v1,const fix_col_vec<T,N>& v2)
{
	

	for(int i = 0;i < N;i++)
		v1(i)-=  v2(i);

	return v1;
}

template <typename T, cgv::type::uint32 N>
fix_row_vec<T,N> operator-(const fix_row_vec<T,N>& v)
{
	T s = -1;
	return s*v;
}

template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> operator-(const fix_col_vec<T,N>& v)
{
	T s = -1;
	return s*v;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
fix_mat<T,N,M> operator-(const fix_mat<T,N,M>& v)
{
	T s = -1;
	return s*v;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> operator-(const fix_sqr_mat<T,N>& v)
{
	T s = -1;
	return s*v;
}
template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N> operator-(const fix_diag_mat<T,N>& v)
{
	T s = -1;
	return s*v;
}

template <typename T, cgv::type::uint32 N>
fix_row_vec<T,N> operator-(const fix_row_vec<T,N>& v1,const fix_row_vec<T,N>& v2)
{
	fix_row_vec<T,N> diff;

	for(int i = 0;i < N;i++)
		diff(i)=v1(i) - v2(i);

	return diff;
}

template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> operator-(const fix_col_vec<T,N>& v1,const fix_col_vec<T,N>& v2)
{
	fix_col_vec<T,N> diff;

	for(int i = 0;i < N;i++)
		diff(i)=v1(i)-v2(i);

	return diff;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
fix_mat<T,N,M> operator-(const fix_mat<T,N,M>& m1,const fix_mat<T,N,M>& m2)
{
	fix_mat<T,N,M> diff;

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j <M;j++)
		{
			diff(i,j)=m1(i,j)-m2(i,j);	
		}
	
	}
	
	return diff;
}

template <typename T, cgv::type::uint32 N>
fix_mat<T,N,N> operator-(const fix_mat<T,N,N>& m1,const fix_diag_mat<T,N>& m2)
{
	fix_mat<T,N,N> diff;

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N;j++)
		{
			diff(i,j)=m1(i,j)-m2(i,j);	
		}
	
	}
	
	return diff;
}

template <typename T, cgv::type::uint32 N>
fix_mat<T,N,N> operator-(const fix_diag_mat<T,N>& m1,const fix_mat<T,N,N>& m2)
{
	fix_mat<T,N,N> diff;

	for(int i = 0; i <N; i++)
	{
		for(int j = 0; j < N;j++)
		{
			diff(i,j)=m1(i,j)-m2(i,j);	
		}
	
	}
	
	return diff;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> operator-(const fix_diag_mat<T,N>& m1,const fix_sqr_mat<T,N>& m2)
{
	fix_sqr_mat<T,N> diff;

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N;j++)
		{
			diff(i,j)=m1(i,j)-m2(i,j);	
		}
	
	}
	
	return diff;
}

template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N> operator-(const fix_diag_mat<T,N>& m1,const fix_diag_mat<T,N>& m2)
{
	fix_diag_mat<T,N> diff;

	for(int i = 0; i < N; i++)
	{
		
		diff(i)=m1(i)-m2(i);	
		
	
	}
	
	return diff;
}



		}
	}
}

