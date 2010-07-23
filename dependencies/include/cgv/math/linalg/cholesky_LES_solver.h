#pragma once
#include <cgv/math/linalg/linalg.h>
#include <cgv/math/linalg/cholesky_factorization.h>
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
template <typename T>
class cholesky_LES_solver
{
	
	int N;	
	cgv::math::linalg::sqr_mat<T> A;
	cgv::math::linalg::col_vec<T> x,y;
	cgv::math::linalg::col_vec<T> b;
	bool posdef_tested,ispd;

	
	

public:

	cholesky_LES_solver(cgv::math::linalg::sqr_mat<T> A, cgv::math::linalg::col_vec<T> b): x(b.size()), y(b.size())
	{
		this->A=A;
		this->b=b;
		N=b.size();

		posdef_tested=false;
		
		
	}

	bool is_positive_definite()
	{
		if(!posdef_tested)
		{
			cgv::math::linalg::cholesky_factorization<T> f(&A);
			posdef_tested=true;
			ispd=f.factorize();
		}
		return ispd;
		
		
	}
	
	

	cgv::math::linalg::col_vec<T> solve()
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

