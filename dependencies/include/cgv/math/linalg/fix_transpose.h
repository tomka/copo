#pragma once
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_sqr_mat.h>
#include <cgv/math/linalg/fix_diag_mat.h>
#include <cgv/math/linalg/fix_sym_mat.h>
#include <cgv/math/linalg/fix_lo_tri_mat.h>
#include <cgv/math/linalg/fix_up_tri_mat.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> transpose(const fix_row_vec<T,N>& v)
{
	return fix_col_vec<T,N>(&v(0));
}

template <typename T, cgv::type::uint32 N>
fix_row_vec<T,N> transpose(const fix_col_vec<T,N>& v)
{
	return fix_row_vec<T,N>(&v(0));
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
fix_mat<T,M,N> transpose(const fix_mat<T,N,M>& m)
{
	fix_mat<T,M,N> mt;

	for(int i = 0; i < m.nrows(); i++)
	{
		for(int j = 0; j < m.ncols();j++)
			mt(j,i)=m(i,j);
		
	}
	return mt;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> transpose(const fix_sqr_mat<T,N>& m)
{
	fix_sqr_mat<T,N> mt;

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N;j++)
			mt(j,i)=m(i,j);
		
	}
	return mt;
}


template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N> transpose(const fix_diag_mat<T,N>& m)
{
	return m;
}

template <typename T, cgv::type::uint32 N>
fix_sym_mat<T,N> transpose(const fix_sym_mat<T,N>& m)
{
	return m;
}
template <typename T, cgv::type::uint32 N>
fix_up_tri_mat<T,N> transpose(const fix_lo_tri_mat<T,N>& m)
{
	fix_up_tri_mat<T,N> um(&(m(0,0)));
	return um;
}
template <typename T, cgv::type::uint32 N>
fix_lo_tri_mat<T,N> transpose(const fix_up_tri_mat<T,N>& m)
{
	fix_lo_tri_mat<T,N> lm(&(m(0,0)));
	return lm;
}

		}
	}
}

