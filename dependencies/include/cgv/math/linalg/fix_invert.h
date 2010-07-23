#pragma once
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_sqr_mat.h>
#include <cgv/math/linalg/fix_diag_mat.h>
#include <cgv/math/linalg/fix_sym_mat.h>
#include <cgv/math/linalg/fix_svd.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{


template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N> invert(const fix_diag_mat<T,N> &m)
{
	fix_diag_mat<T,N> r;
	for(int i = 0; i < N;i++)
	{	
		r(i)=1/m(i);
	}
	return r;
}


template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> invert(const fix_sqr_mat<T,N> &m)
{
	fix_mat<T,N,N> a = m;
	fix_diag_mat<T,N> w(N);
	fix_mat<T,N,N> v;
	svd(a,w,v);

	a = v*invert(w)*transpose(a);
	fix_sqr_mat<T,N> inv(&(a(0,0)));
	
			
	return inv;

}

template <typename T, cgv::type::uint32 N>
fix_sym_mat<T,N> invert(const fix_sym_mat<T,N> &m)
{
	fix_mat<T,N,N> a = m;
	fix_diag_mat<T,N> w(N);
	fix_mat<T,N,N> v;
	svd(a,w,v);

	a = v*invert(w)*transpose(a);
	fix_sym_mat<T,N> inv;
	for(int i = 0; i < N;i++)
		for(int j = i;j < N;j++)
			inv(i,j) = a(i,j);
	
			
	return inv;

}
			
		}
	}
}

