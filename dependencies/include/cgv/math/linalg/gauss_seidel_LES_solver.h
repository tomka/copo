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
template <typename T>
class gauss_seidel_LES_solver
{
	int N;	
	

	cgv::math::linalg::sqr_mat<T> A;
	cgv::math::linalg::col_vec<T> x;
	cgv::math::linalg::col_vec<T> b;

	

public:
	T epsilon;
	int maxiterations;

	gauss_seidel_LES_solver(cgv::math::linalg::sqr_mat<T> A, cgv::math::linalg::col_vec<T> b):x(b.size())
	{
		this->A=A;
		this->b=b;
		N=b.size();
		epsilon=0.001f;
		maxiterations=1000;
	}


	
	

	cgv::math::linalg::col_vec<T> solve()
	{
	
		//cgv::math::linalg::col_vec<T> x((T)0,b.size());
		

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
