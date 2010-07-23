#pragma once
#include <cmath>
#include <cgv/math/linalg/col_vec.h>
#include <cgv/math/linalg/row_vec.h>
#include <cgv/math/linalg/length.h>
#include <cgv/math/linalg/scl_mult.h>


namespace cgv{
	namespace math{
		namespace linalg{
///return a normalize a column vector
template <class T>
col_vec<T> normalize(const col_vec<T> &v)
{
	T l = (T)1.0/length(v);
	return 	l*v;
}

template <class T>
row_vec<T> normalize(const row_vec<T> &v)
{
	T l = (T)1.0/length(v);
	return 	l*v;
}
		}
	}
}

