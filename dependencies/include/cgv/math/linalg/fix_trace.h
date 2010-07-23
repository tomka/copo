#pragma once

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
T trace(const fix_sqr_mat<T,N> &m)
{
	T t = 0;
	for(int i = 0; i < m.nrows();i++)
		t+=m(i,i);
	return t;
}

template <typename T, cgv::type::uint32 N>
T trace(const fix_diag_mat<T,N> &m)
{
	T t = 0;
	for(int i = 0; i < m.nrows();i++)
		t+=m(i,i);
	return t;
}

template <typename T, cgv::type::uint32 N>
T trace(const fix_sym_mat<T,N> &m)
{
	T t = 0;
	for(int i = 0; i < m.nrows();i++)
		t+=m(i,i);
	return t;
}
template <typename T, cgv::type::uint32 N>
T trace(const fix_lo_tri_mat<T,N> &m)
{
	T t = 0;
	for(int i = 0; i < m.nrows();i++)
		t+=m(i,i);
	return t;
}
template <typename T, cgv::type::uint32 N>
T trace(const fix_up_tri_mat<T,N> &m)
{
	T t = 0;
	for(int i = 0; i < m.nrows();i++)
		t+=m(i,i);
	return t;
}

		}
	}
}

