#pragma once
#include <cgv/math/linalg/col_vec.h>

namespace cgv {
	namespace math {
		namespace linalg {

template<class T>
T dot(const cgv::math::linalg::col_vec<T> &v1,const cgv::math::linalg::col_vec<T> &v2)
{
	T s=0;
	for(int i = 0; i < v1.size();i++)
		s+=v1(i)*v2(i);
	return s;
}

template<class T>
cgv::math::linalg::col_vec<T> cross(const cgv::math::linalg::col_vec<T> &v1,const cgv::math::linalg::col_vec<T> &v2)
{
	cgv::math::linalg::col_vec<T> c(3);
	c(0) =  v1(1)*v2(2) - v1(2)*v2(1);
    c(1) = -v1(0)*v2(2) + v1(2)*v2(0);
	c(2) =  v1(0)*v2(1) - v1(1)*v2(0);
	return c;
}

template<class T>
cgv::math::linalg::col_vec<T> dbl_cross(const cgv::math::linalg::col_vec<T> &a,const cgv::math::linalg::col_vec<T> &b, cgv::math::linalg::col_vec<T> &c)
{
	return dot(a,c)*b - dot(a,b)*c;
}

template<class T>
T spat(const cgv::math::linalg::col_vec<T> &a,const cgv::math::linalg::col_vec<T> &b,const cgv::math::linalg::col_vec<T> &c)
{
	 return dot(cross(a,b),c);
}


		}
	}
}

