#pragma once
#include <cgv/math/linalg/linalg.h>
#include <numeric>

namespace cgv{
	namespace math{
		namespace linalg{


/**
* Solver for linear equation system A*x + b = 0.
* Implements iterative gauss seidel algorithm.
* Input: matrix A and vector b; A should be strict diagonal dominant to ensure convergence!
* Result: vector x
*/
template <typename T, int N>
class fix_gauss_seidel_LES_solver
{
	
	

	cgv::math::linalg::fix_sqr_mat<T,N> A;
	cgv::math::linalg::fix_col_vec<T,N> x;
	cgv::math::linalg::fix_col_vec<T,N> b;

	

public:
	T epsilon;
	int maxiterations;

	fix_gauss_seidel_LES_solver(cgv::math::linalg::fix_sqr_mat<T,N> A, cgv::math::linalg::fix_col_vec<T,N> b)
	{
		this->A=A;
		this->b=b;
		epsilon=0.001f;
		maxiterations=1000;
	}


	
	

	cgv::math::linalg::fix_col_vec<T,N> solve()
	{
	
		cgv::math::linalg::fix_col_vec<T,N> x((T)0);
		

		T normr=1;
		T normb=std::abs(b(0)); 
		for(int i = 1; i < N;i++)
			normb = std::max(normb,std::abs(b(i)));
		
		int it=0; 

		while ((it++ < maxiterations) && (normr/normb>epsilon)) 
		{ 
			normr=0; 
			for (int i=0;i<N;i++) 
			{ 
	            T c=-b(i); 
				for (int j=0;j<N;j++) 
				{ 
					c -= A(i,j)*x(j); 
				} 
				normr = std::max(std::abs(c),normr); 
				x(i) = x(i)+c/A(i,i); 
         } 
         
      } 


		return x;
	}
};
		}
	}
}
