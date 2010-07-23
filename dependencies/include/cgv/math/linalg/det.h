#pragma once
#include <cgv/math/linalg/col_vec.h>
#include <cgv/math/linalg/row_vec.h>
#include <cgv/math/linalg/mat.h>
#include <cgv/math/linalg/sqr_mat.h>
#include <cgv/math/linalg/diag_mat.h>
#include <cgv/math/linalg/sym_mat.h>
#include <cgv/math/linalg/up_tri_mat.h>
#include <cgv/math/linalg/lo_tri_mat.h>
#include <cgv/math/linalg/svd.h>
#include <math.h>



namespace cgv
{
	namespace math
	{
		namespace linalg
		{

///calculates the determinat of diagonial matrix
template <class T>
T det(const diag_mat<T> &m)
{
	T p = 1;

	for(int i = 0; i<m.ncols();i++)
		p*=m(i);

	return p;
}

template <class T>
T det(const lo_tri_mat<T> &m)
{
	T p = 1;

	for(int i = 0; i<m.ncols();i++)
		p*=m(i,i);

	return p;
}


	
template <class T>
T det(const up_tri_mat<T> &m)
{
	T p = 1;

	for(int i = 0; i<m.ncols();i++)
		p*=m(i,i);

	return p;
}




///calculates the determinant of square matrix (recursively)
template <class T>
T det(const sqr_mat<T> &mat)
{
	
	T d = 0.0;
	if ( mat.nrows() == 1 )
		return mat(0,0);
    if ( mat.nrows() == 2 )
    {
		d = mat(0,0) * mat(1,1) - mat(0,1) * mat(1,0);
		return d;
    }
    int  j, m, n, k, l;
    sqr_mat<T> temp(mat.nrows() - 1);
    
	for ( j = 0; j < mat.ncols(); j++)
    {
		m = 0;
        n = 0;
        for ( k = 1; k < mat.nrows(); k++ )
        {
			for ( l = 0; l < mat.ncols(); l++ )
            {
				if ( l != j)
                {
					temp(m,n)=mat(l,k);
					if ( n < mat.ncols() - 2)
						n++;
					else
					{
						n = 0;
						m++;
					}
				}
			}
		}
		if (mat(j,0) != 0)
			d = d + pow(-1.0f, j) * mat(j,0) * det(temp);
	}
	return d;
}

template <class T>
T det(const sym_mat<T> &mat)
{
	
	T d = 0;
	if ( mat.nrows() == 1 )
		return mat(0,0);
    if ( mat.nrows() == 2 )
    {
		d = mat(0,0) * mat(1,1) - mat(0,1) * mat(1,0);
		return d;
    }
    int  j, m, n, k, l;
    sym_mat<T> temp(mat.nrows() - 1, mat.ncols() - 1);
    
	for ( j = 0; j < mat.ncols(); j++)
    {
		m = 0;
        n = 0;
        for ( k = 1; k < mat.nrows(); k++ )
        {
			for ( l = 0; l < mat.ncols(); l++ )
            {
				if ( l != j)
                {
					temp(m,n)=mat(l,k);
					if ( n < mat.ncols() - 2)
						n++;
					else
					{
						n = 0;
						m++;
					}
				}
			}
		}
		d = d + pow(-1.0, j) * mat(j,0) * det(temp);
	}
     return d;
}



		}
	}
}

