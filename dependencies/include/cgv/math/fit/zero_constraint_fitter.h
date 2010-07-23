#pragma once
#include <newmat.h>
#include <newmatap.h>
#include <vector>
#include <iterator>

namespace cgv {
	namespace math {
		namespace fit {

/** the zero constaint fitter is used to find non degenerate solutions of linear equation systems 
    of the form 
	\f{eqnarray*}
		\alpha_{1,1}x_1+\alpha_{1,2}x_2+\ldots+\alpha_{1,n}x_n & = & 0\\
		\alpha_{2,1}x_1+\alpha_{2,2}x_2+\ldots+\alpha_{2,n}x_n & = & 0\\
		\vdots & \vdots & \vdots\\
		\alpha_{m,1}x_1+\alpha_{m,2}x_2+\ldots+\alpha_{m,n}x_n & = & 0
	\f}
	or in matrix notation \f$A\vec x=0\f$. Where n and m are independent positive integers. The 
	zero_constraint_fitter allows to compute a non trivial solution for \f$\vec x\f$. This is 
	achieved with a eigenvalue decomposition of \f$ A^TA\f$. The solution is taken from the 
	eigenvector corresponding to the smallest eigenvalue. The smallest eigenvalue is not 
	necessarily unique. Therefore, the zero_constraint_fitter has methods to query the 
	dimensionality of the solution space and to query every single eigenvalue and eigenvector.

	The problem is set up by specifying the rows of the matrix \f$A\f$ with the methods \c add_constraint 
	or \c add_weighted_constraint. The latter will multiply the contribution to \f$ A^TA\f$ with the 
	given weight, what is equivalent to multiplication of the \f$\alpha_{i,j}\f$ with the square root
	of the given weight, if the weight is positive.

	To solve the problem one can either directly call \c solve, which will output some solution. The
	other option is to call \c prepare_solution and use the methods \c get_dimension_of_solution_space
	and \c get_eigenvalue. The latter method also allows to query the corresponding eigenvector. 
*/
template <typename ta_value_type>
class zero_constraint_fitter
{
public:
	/// 
	typedef ta_value_type value_type;
	/// 
	typedef unsigned int size_type;
protected:
	/// store the used dimension
	size_type dimension;
	/// epsilon used to analyze the solution space
	value_type epsilon;
	/// store covariance matrix
	NEWMAT::SymmetricMatrix C;
	/// store the eigenvalues
	NEWMAT::DiagonalMatrix D;
	/// and eigenvectors
	NEWMAT::Matrix O;
public:
	/// construct from dimension of matrix
	zero_constraint_fitter(size_type dim=4) : epsilon(1e-8)
	{
		set_dimension(dim);
	}
	/// set a different dimension
	void set_dimension(size_type dim)
	{
		dimension = dim;
		C = NEWMAT::SymmetricMatrix(dim);
		initialize();
	}
	/// remove all contraints
	void initialize()
	{
		C = 0;
	}
	/// add a constraint
	template <class ta_iterator>
	void add_contraint(ta_iterator iterator)
	{
		for (int j = 0; j < dimension; ++j) {
			for (int k = j; k < dimension; ++k)
				C(j+1,k+1) += (value_type)(*(iterator+j))* (value_type)(*(iterator+k));
		}
	}
	/// add a weighted constraint, where the input values are given by a random access iterator
	template <class ta_weight_type, class ta_iterator>
	void add_weighted_contraint(ta_weight_type weight, ta_iterator iterator)
	{
		for (int j = 0; j < dimension; ++j)
			for (int k = j; k < dimension; ++k)
				C(j+1,k+1) += weight * (double)(*(iterator+j))* (double)(*(iterator+k));
	}
	/// prepare the solution by computing the eigenvector decomposition
	void prepare_solution()
	{
		/// compute SDV of matrix C
		D = NEWMAT::DiagonalMatrix(dimension); D = 0;
		O = NEWMAT::Matrix(dimension,dimension); O = 0;
		NEWMAT::Jacobi(C,D,O);
	}
	/// return the dimension of the solution space and store the dimension indices in the memory pointed to by the output iterator
	template <class ta_iterator>
	size_type get_dimension_of_solution_space(ta_iterator* output_iterator_pointer) const
	{
		size_type i;
		size_type solution_space_dimension = 0;
		// compute index of minimum eigenvalue first
		size_type index_of_minimum=1;
		for (i=2; i<=dimension; ++i) {
			if (fabs(D(i,i)) < fabs(D(index_of_minimum,index_of_minimum))) 
				index_of_minimum = i;
		}
		// iterate over the eigenvalues again
		for (i=1; i<=dimension; ++i) {
			if (fabs(D(i,i)) < epsilon ||
				(fabs(D(i,i)-D(index_of_minimum,index_of_minimum)) < epsilon*D(index_of_minimum,index_of_minimum))) {
					++solution_space_dimension;
					if (output_iterator_pointer) {
						*(*output_iterator_pointer) = i-1;
						++(*output_iterator_pointer);
					}
			}
		}
		return solution_space_dimension;
	}
	/// return the dimension of the solution space
	size_type get_dimension_of_solution_space() const
	{
		int** output_iterator = 0;
		return get_dimension_of_solution_space(output_iterator);
	}
	/// return the eigenvalue of a given dimension
	value_type get_eigenvalue(int dim) const
	{
		return (value_type) D(dim+1);
	}
	/// extract the eigenvector of a given dimension to the iterator pointed to by \c output_eigenvector_iterator_pointer and return the corresponding eigenvalue
	template <class ta_iterator>
	value_type get_eigenvalue(int dim, ta_iterator* output_eigenvector_iterator_pointer) const
	{
		if (output_eigenvector_iterator_pointer) {
			for (int i=1; i<=dimension; ++i)
				*(*output_eigenvector_iterator_pointer) = O(i,dim);
		}
		return get_eigenvalue(dim);
	}
	/// solve the fitting problem, by calling prepare_solution and extracting a mixture of the eigenvectors in the solution space to the output iterator
	template <class ta_iterator>
	void solve(ta_iterator output_iterator)
	{
		prepareSolution();
		// extract weight vector
		std::vector<size_type> solutionDimensions;
		std::back_insert_iterator<std::vector<size_type> > back_insert(solutionDimensions);
		size_type solutionDimension = getDimensionOfSolutionSpace(&back_insert);
		NEWMAT::ColumnVector Z(dimension); 
		Z = 0;
		size_type i;
		for (i=0; i<solutionDimension; ++i)
			Z(solutionDimensions[i]+1) = 1;
		/// extract eigenvector
		NEWMAT::ColumnVector alpha(dimension); 
		alpha=0;
		alpha=O*Z;
		/// copy result to output
		for (i = 1; i <= dimension; ++i, ++output_iterator)
			*output_iterator = alpha(i);
	}
};
		}
	}
}
