#pragma once
#include <cgv/math/linalg/col_vec.h>
#include <cgv/math/linalg/row_vec.h>
#include <cgv/math/linalg/mat.h>
#include <cgv/math/linalg/sqr_mat.h>
#include <cgv/math/linalg/diag_mat.h>
#include <cgv/math/linalg/svd.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <class T>
diag_mat<T> invert(const diag_mat<T> &m)
{
	diag_mat<T> r(m.nrows());
	for(int i = 0; i < m.nrows();i++)
	{	
		r(i)=1/m(i);
	}
	return r;
}

template<class T>
sqr_mat<T> invert(sqr_mat<T> &m)
{
	mat<T> a = m;
	diag_mat<T> w(a.nrows());
	mat<T> v(a.nrows(),a.nrows());
	svd(a,w,v);

	a = v*invert(w)*transpose(a);
	sqr_mat<T> inv(&(a(0,0)),a.ncols());
	
			
	return inv;

}
			
		}
	}
}

