#pragma once
#include <cmath>
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_length.h>
#include <cgv/math/linalg/fix_scl_mult.h>


namespace cgv{
	namespace math{
		namespace linalg{
///return a normalize a column vector
template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> normalize(const fix_col_vec<T,N> &v)
{
	T l = (T)1.0/length(v);
	return 	l*v;
}

template <typename T, cgv::type::uint32 N>
fix_row_vec<T,N> normalize(const fix_row_vec<T,N> &v)
{
	T l = (T)1.0/length(v);
	return 	l*v;
}
		}
	}
}

