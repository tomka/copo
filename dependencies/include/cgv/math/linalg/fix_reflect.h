#pragma once
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/type/size_type.h>


namespace cgv {
	namespace math {
		namespace linalg {


template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> reflect(const fix_col_vec<T,N> &v, const fix_col_vec<T,N> &n)
{
	return v-2*dot(v,n)*n;
}
		}
	}
}
