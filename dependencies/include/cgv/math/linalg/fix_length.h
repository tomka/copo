#pragma once
#include <cmath>
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <typename T, cgv::type::uint32 N>
T length(const fix_col_vec<T,N> v)
{
	T s = 0;
	for(int i = 0;i < N;i++)
	{
		s+=v(i)*v(i);
	}
	return sqrt(s);
}

template <typename T, cgv::type::uint32 N>
T sqr_length(const fix_col_vec<T,N> v)
{
	T s = 0;
	for(int i = 0;i < N;i++)
	{
		s+=v(i)*v(i);
	}
	return s;
}

template <typename T, cgv::type::uint32 N>
T length(const fix_row_vec<T,N> v)
{
	T s = 0;
	for(int i = 0;i < N;i++)
	{
		s+=v(i)*v(i);
	}
	return sqrt(s);
}

template <typename T, cgv::type::uint32 N>
T sqr_length(const fix_row_vec<T,N> v)
{
	T s = 0;
	for(int i = 0;i < N;i++)
	{
		s+=v(i)*v(i);
	}
	return s;
}


		}
	}
}
