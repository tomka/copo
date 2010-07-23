#pragma once
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_sqr_mat.h>
#include <cgv/math/linalg/fix_diag_mat.h>
#include <cgv/math/linalg/fix_sym_mat.h>
#include <cgv/math/linalg/fix_lo_tri_mat.h>
#include <cgv/math/linalg/fix_up_tri_mat.h>
#include <cgv/math/linalg/fix_svd.h>
#include <cgv/math/linalg/det.h>
#include <math.h>



namespace cgv
{
	namespace math
	{
		namespace linalg
		{

///calculates the determinant of diagonial matrix
template <typename T, cgv::type::uint32 N>
T det(const fix_diag_mat<T,N> &m)
{
	T p = 1;

	for(int i = 0; i<m.ncols();i++)
		p*=m(i);

	return p;
}

///calculates the determinant of upper triangular  matrix
template <typename T, cgv::type::uint32 N>
T det(const fix_up_tri_mat<T,N> &m)
{
	T p = 1;

	for(int i = 0; i<m.ncols();i++)
		p*=m(i,i);

	return p;
}


template <typename T, cgv::type::uint32 N>
T det(const fix_lo_tri_mat<T,N> &m)
{
	T p = 1;

	for(int i = 0; i<m.ncols();i++)
		p*=m(i,i);

	return p;
}







template <typename T, cgv::type::uint32 N>
T det(const fix_mat<T,N,N> &m)
{
	sqr_mat<T> cpy_mat(N);
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N;j++)
			cpy_mat(i,j) = m(i,j);
	return det(cpy_mat);

}

template <typename T, cgv::type::uint32 N>
T det(const fix_sqr_mat<T,N> &m)
{
	sqr_mat<T> cpy_mat(N);
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N;j++)
			cpy_mat(i,j) = m(i,j);
	return det(cpy_mat);
	
}


template <typename T, cgv::type::uint32 N>
T det(const fix_sym_mat<T,N> &m)
{
	sqr_mat<T> cpy_mat(N,N);
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N;j++)
			cpy_mat(i,j) = m(i,j);
	return det(cpy_mat);
}


		}
	}
}

