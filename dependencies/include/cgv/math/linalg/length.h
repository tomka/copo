#pragma once
#include <cmath>
#include <cgv/math/linalg/col_vec.h>
#include <cgv/math/linalg/row_vec.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template<class T>
T length(const col_vec<T> v)
{
	T s = 0;
	for(int i = 0;i < v.size();i++)
	{
		s+=v(i)*v(i);
	}
	return sqrt(s);
}

template<class T>
T sqr_length(const col_vec<T> v)
{
	T s = 0;
	for(int i = 0;i < v.size();i++)
	{
		s+=v(i)*v(i);
	}
	return s;
}

template<class T>
T length(const row_vec<T> v)
{
	T s = 0;
	for(int i = 0;i < v.size();i++)
	{
		s+=v(i)*v(i);
	}
	return sqrt(s);
}

template<class T>
T sqr_length(const row_vec<T> v)
{
	T s = 0;
	for(int i = 0;i < v.size();i++)
	{
		s+=v(i)*v(i);
	}
	return s;
}


		}
	}
}
