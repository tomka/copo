#pragma once
#include <cgv/math/linalg/linalg.h>
#include <numeric>

namespace cgv{
	namespace math{
		namespace linalg{


/**
* Calculate a cholesky factorization of a given square matrix A.
* A = L*L^T 
* A has to be symmetric positive definite
*/
template <typename T, int N>
class fix_cholesky_factorization
{
	cgv::math::linalg::fix_sqr_mat<T,N> *A;
		

public:
	

	fix_cholesky_factorization(cgv::math::linalg::fix_sqr_mat<T,N> *A)
	{
		this->A=A;
	}


	
	
	//return true if matrix is positive definite
	bool factorize()
	{
				
		
		for(int i = 0; i < N;i++)
		{
			
			for(int j = i; j < N;j++)
			{
				T sum= (*A)(i, j);
				for(int k = i - 1; k >= 0;k--)
				{
					sum -=  (*A)(i, k) * (*A)(j, k);
				}
 
				if(i == j)
				{
					if(sum <= 0)
						return false;//not positive definite
					else
						(*A)(j, i) = sqrt(sum); 
				}
				else
					(*A)(j, i) = sum / (*A)(i, i);
			}
		}

		//zero out upper diagonal elements
		for(int i = 0; i < N;i++)
		{
			for(int j = 0; j < i;j++)
			{
				(*A)(j,i) = 0;
			}
		}
   

		return true;			
	}


};
		}
	}
}

