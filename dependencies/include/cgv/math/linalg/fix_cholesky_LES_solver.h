#pragma once
#include <cgv/math/linalg/linalg.h>
#include <cgv/math/linalg/fix_cholesky_factorization.h>
#include <vector>

namespace cgv{
	namespace math{
		namespace linalg{


/**
* Solver for linear equation system A*x + b = 0.
* Uses cholesky factorization.
* Input: matrix A and  vector b, A has to be symmetric positive definite
* Result: vector x
*/
template <typename T, int N>
class fix_cholesky_LES_solver
{
	
	
	cgv::math::linalg::fix_sqr_mat<T,N> A;
	cgv::math::linalg::fix_col_vec<T,N> x,y;
	cgv::math::linalg::fix_col_vec<T,N> b;
	bool posdef_tested,ispd;

	
	

public:

	fix_cholesky_LES_solver(cgv::math::linalg::fix_sqr_mat<T,N> A, cgv::math::linalg::fix_col_vec<T,N> b)
	{
		this->A=A;
		this->b=b;
		posdef_tested=false;
		
		
	}

	bool is_positive_definite()
	{
		if(!posdef_tested)
		{
			cgv::math::linalg::fix_cholesky_factorization<T,N> f(&A);
			posdef_tested=true;
			ispd=f.factorize();
		}
		return ispd;
		
		
	}
	
	

	cgv::math::linalg::fix_col_vec<T,N> solve()
	{
		
		if(!is_positive_definite())
			std::cerr << "matrix is not positive definite"<<std::endl;
		
		
		T term;
		
		for(int n=0;n<N;n++)
		{
			term = 0;

			for(int j = 0; j < n; j++)
			{
				term += A(n,j) * y(j);
			}
			
			y(n) = -(term + b(n)) / (A(n,n));
			
			
		}
		
				

		for(int n=N-1;n>=0;n--)
		{
			term = 0;

			for(int j = n+1; j < N; j++)
			{
				term += A(j,n) * x(j);
			}
			
			x(n) = -(term - y(n)) / (A(n,n));
			
			
		}
		
		

		
		return x;
	}
};
		}
	}
}

