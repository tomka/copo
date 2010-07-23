#pragma once
#include <cgv/math/linalg/linalg.h>
#include <vector>

namespace cgv{
	namespace math{
		namespace linalg{


/**
* Solver for linear equation system A*x + b = 0.
* Uses gauss elemination (LU-decomposition) with column pivoting to provide better stability.
* Input: matrix A and  vector b
* Result: vector x
*/
template <typename T, int N>
class fix_gauss_LES_solver
{
	
	//row permutation indices
	std::vector<int> rind;

	cgv::math::linalg::fix_sqr_mat<T,N> A;
	cgv::math::linalg::fix_col_vec<T,N> x;
	cgv::math::linalg::fix_col_vec<T,N> b;

	//find best column pivot element
	int find_pivot(int i)
	{
		int index =i;

		T relpivotsize_index=0;
		for(int c = i; c < N; c++)
		{
		   relpivotsize_index += std::abs(A(rind[index],c));  
		}
		relpivotsize_index = std::abs(A(rind[index],i))/relpivotsize_index;


		for(int c = i+1; c < N; c++)
		{
			T relpivotsize_c=0;
			for(int k = i; k < N; k++)
			{
				relpivotsize_c += std::abs(A(rind[c],k));  
			}

			relpivotsize_c = std::abs(A(rind[c],i))/relpivotsize_c;

			if(relpivotsize_c > relpivotsize_index)
			{
				index = c;
				relpivotsize_index = relpivotsize_c;
			}
		}
		return index;
	}

public:

	fix_gauss_LES_solver(cgv::math::linalg::fix_sqr_mat<T,N> A, cgv::math::linalg::fix_col_vec<T,N> b):rind(N)
	{
		this->A=A;
		this->b=b;

		for(int i=0; i< N;i++)
			rind[i]=i;

		
		
	}
	
	

	cgv::math::linalg::fix_col_vec<T,N> solve()
	{
	
		int pivotindex;
		T lj;
    
	    for(int i = 0; i < N; i++)
	    {
			pivotindex = find_pivot(i);
			
			if(i != pivotindex)
			{
				int j = rind[pivotindex];
				rind[pivotindex]=rind[i];            
				rind[i]=j;            
			}

			for(int j = i+1;j < N; j++)
			{	
				if(A(rind[j],i) != 0)
				{
					lj = (A(rind[j],i))/(A(rind[i],i));
	                
					for(int k = i;k < N; k++)
					{
						A(rind[j],k) -=  lj * A(rind[i],k);
					}

					b(rind[j]) -= lj * b(rind[i]);
				}
			}
		}
		T term;
		
		for(int n=N-1;n>=0;n--)
		{
			term = 0;

			for(int j = n+1; j < N; j++)
			{
				term += A(rind[n],j) * x(j);
			}
			
			x(n) = -(term + b(rind[n])) / (A(rind[n],n));
			
			
		}

		
	
		return x;
	}
};
		}
	}
}
