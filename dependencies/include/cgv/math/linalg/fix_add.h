#pragma once
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_sqr_mat.h>
#include <cgv/math/linalg/fix_quaternion.h>


namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <typename T>
fix_quaternion<T> operator+=(fix_quaternion<T>& q1,const fix_quaternion<T>& q2)
{
	for(int i = 0;i < 4;i++)
		q1(i) +=  q2(i);

	return q1;
}

template <typename T>
fix_quaternion<T> operator+(const fix_quaternion<T>& v1,const fix_quaternion<T>& v2)
{
	fix_quaternion<T> sum(0.0f,0.0f,0.0f,0.0f);

	for(int i = 0;i < 4;i++)
		sum(i)=v1(i) + v2(i);

	return sum;
}

template <typename T, cgv::type::uint32 N>
fix_row_vec<T,N> operator+=(fix_row_vec<T,N>& v1,const fix_row_vec<T,N>& v2)
{
	for(int i = 0;i < N;i++)
		v1(i)+=  v2(i);

	return v1;
}

template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> operator+=(fix_col_vec<T,N>& v1,const fix_col_vec<T,N>& v2)
{
	for(int i = 0;i < N;i++)
		v1(i)+=  v2(i);

	return v1;
}

template <typename T, cgv::type::uint32 N>
fix_row_vec<T,N> operator+(const fix_row_vec<T,N>& v1,const fix_row_vec<T,N>& v2)
{
	fix_row_vec<T,N> sum(T(0));

	for(int i = 0;i < N;i++)
		sum(i)=v1(i) + v2(i);

	return sum;
}

template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> operator+(const fix_col_vec<T,N>& v1,const fix_col_vec<T,N>& v2)
{
	fix_col_vec<T,N> sum(T(0));

	for(int i = 0;i < N;i++)
		sum(i)=v1(i)+v2(i);

	return sum;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
fix_mat<T,N,M> operator+(const fix_mat<T,N,M>& m1,const fix_mat<T,N,M>& m2)
{
	fix_mat<T,N,M> sum(T(0));

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < M;j++)
		{
			sum(i,j)=m1(i,j)+m2(i,j);	
		}
	
	}
	
	return sum;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> operator+(const fix_sqr_mat<T,N>& m1,const fix_sqr_mat<T,N>& m2)
{
	fix_sqr_mat<T,N> sum(T(0));

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N;j++)
		{
			sum(i,j)=m1(i,j)+m2(i,j);	
		}
	
	}
	
	return sum;
}

		}
	}
}
