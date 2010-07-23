#pragma once

#include <cgv/math/linalg/sqr_mat.h>
#include <cgv/math/linalg/diag_mat.h>
#include <cgv/math/linalg/sym_mat.h>
#include <cgv/math/linalg/lo_tri_mat.h>
#include <cgv/math/linalg/up_tri_mat.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template<class T>
T trace(const sqr_mat<T> &m)
{
	T t = 0;
	for(int i = 0; i < m.nrows();i++)
		t+=m(i,i);
	return t;
}

template<class T>
T trace(const diag_mat<T> &m)
{
	T t = 0;
	for(int i = 0; i < m.nrows();i++)
		t+=m(i,i);
	return t;
}

template<class T>
T trace(const sym_mat<T> &m)
{
	T t = 0;
	for(int i = 0; i < m.nrows();i++)
		t+=m(i,i);
	return t;
}

template<class T>
T trace(const up_tri_mat<T> &m)
{
	T t = 0;
	for(int i = 0; i < m.nrows();i++)
		t+=m(i,i);
	return t;
}

template<class T>
T trace(const lo_tri_mat<T> &m)
{
	T t = 0;
	for(int i = 0; i < m.nrows();i++)
		t+=m(i,i);
	return t;
}







		}
	}
}

