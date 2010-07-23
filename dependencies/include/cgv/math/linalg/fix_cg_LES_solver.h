#pragma once
#include <cgv/math/linalg/linalg.h>
#include <numeric>
#pragma warning (disable:4003)

namespace cgv{
	namespace math{
		namespace linalg{


/**
* Solver for linear equation system A*x + b = 0.
* Implements iterative conjugate gradient method without preconditioning.
* Input: matrix A and vector b; A has to be hermitian and positive definite!
* Result: vector x
*/
template <typename T, int N>
class fix_cg_LES_solver
{
	
	

	cgv::math::linalg::fix_sqr_mat<T,N> A;
	cgv::math::linalg::fix_col_vec<T,N> x;
	cgv::math::linalg::fix_col_vec<T,N> b;

	

public:
	T epsilon;
	int maxiterations;

	fix_cg_LES_solver(cgv::math::linalg::fix_sqr_mat<T,N> A, cgv::math::linalg::fix_col_vec<T,N> b)
	{
		this->A=A;
		this->b=b;
		epsilon=0.0001f;
		maxiterations=1000;
	}


	
	

	cgv::math::linalg::fix_col_vec<T,N> solve()
	{

	
		cgv::math::linalg::fix_col_vec<T,N> x0((T)0),x1;
		cgv::math::linalg::fix_col_vec<T,N> r0,r1,d0,d1; 
		
		
		
		    
		T error = std::numeric_limits<T>::max();
		int iter = 0;
		T alpha_k, beta_k;
		T sum=0;
		T temp=0;

	    
		r0 = b - A*x0;
		d0 = r0;
	    
	    
		while (sqr_length(r0) > epsilon   && iter++ < maxiterations)
		{
				        
			cgv::math::linalg::fix_col_vec<T,N> Ad = A * d0;
			T r0sqrlength = sqr_length(r0);
			if(r0sqrlength == 0)
			{   
				std::cerr <<"error:" <<error<<std::endl;
				return x0;
			}
			
			alpha_k = r0sqrlength / dot(d0, Ad);
	        
			x1 = x0 + alpha_k * d0;
	        
			r1 = r0 - alpha_k * Ad;
			
			beta_k = sqr_length(r1)/r0sqrlength;

			d1 = r1 + beta_k * d0;
	        
			error = 0;
			for(int i = 0; i < N; i++)
			{
				
				sum = 0;
				for(int j = 0; j < i; j++)
					sum += A(i,j)*x1(j);
				
				for(int j = i+1; j < N; j++)
					sum += A(i,j)*x1(j);
				
				x1(i) = (-b(i) - sum)/A(i,i);

				//error calculation
				temp = fabs(x1(i)-x0(i));

				if(temp > error)
					error = temp;
					
			}
			x0 = x1;
			r0 = r1;
			d0 = d1;
							
		}
		return x0;
	}





};
		}
	}
}

