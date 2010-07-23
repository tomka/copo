#pragma once
#include <cgv/math/linalg/fix_col_vec.h>

namespace cgv {
	namespace math {
		namespace linalg {

template <typename T, cgv::type::uint32 N>
T dot(const fix_col_vec<T,N> &v1,const fix_col_vec<T,N> &v2)
{
	T s=0;
	for(int i = 0; i < N;i++)
		s+=v1(i)*v2(i);
	return s;
}

template <typename T>
fix_col_vec<T,3> cross(const fix_col_vec<T,3> &v1,const fix_col_vec<T,3> &v2)
{
	fix_col_vec<T,3> c(3);
	c(0) =  v1(1)*v2(2) - v1(2)*v2(1);
    c(1) = -v1(0)*v2(2) + v1(2)*v2(0);
	c(2) =  v1(0)*v2(1) - v1(1)*v2(0);
	return c;
}

template <typename T>
fix_col_vec<T,3> dbl_cross(const fix_col_vec<T,3> &a,const fix_col_vec<T,3> &b, fix_col_vec<T,3> &c)
{
	return dot(a,c)*b - dot(a,b)*c;
}

template <typename T>
T spat(const fix_col_vec<T,3> &a,const fix_col_vec<T,3> &b,const fix_col_vec<T,3> &c)
{
	 return dot(cross(a,b),c);
}

		}
	}
}
