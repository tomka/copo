#pragma once

#include <cgv/storage/collection/array.h>
#include <cgv/range/operators.h>
#include <cgv/storage/view/triangular_view.h>

namespace cgv {
	namespace math {
		namespace func {
			/** implementation of a multi-dimensional taylor basis. It uses the triangular storage view
			    to organize the basis linearly in memory. The basis class does not keep the memory for the
				basis, but only provides algorithms to compute the basis. For a dimensionality of two and degree
				\f$d\f$, the basis is composed of all terms \f$x^iy^j\f$ with \f$0\leq i+j\leq d\f$. The order
				is \f$1,x,x^2,\ldots,x^d,y,xy,x^2y,\ldots,x^{d-1}y,\ldots,y^d\f$. In three dimension the basis
				is composed of all terms \f$x^iy^jz^k\f$ with \f$0\leq i+j+k\leq d\f$ with correponding ordering.
				A specialization not derived from the triangular_view is implemented for the one dimensional
				basis \f$1,x,x^2,\ldots,x^d\f$. The important functionality of this class is the evaluation of 
				the basis in \f$O(n)\f$, where \f$n\f$ is the number of terms in the basis, which can be queried
				with \c get_size
				*/
			template <typename ta_source_value, cgv::type::size_type ta_dimensionality = 1>
			class taylor_basis : protected cgv::storage::view::triangular_view<ta_dimensionality>
			{
				/// type of base class
				typedef typename cgv::storage::view::triangular_view<ta_dimensionality> base_type;
			public:
				/// type of a vector that holds the exponents of a combined monom
				static const size_type dimensionality = ta_dimensionality;
				typedef typename base_type::multi_index_type exponent_type;
				typedef ta_source_value source_value_type;
				typedef typename cgv::storage::collection::array<source_value_type,ta_dimensionality> source_type;
				/// construct basis from given degree
				taylor_basis(size_type degree) { set_degree(degree); }
				/// return the exponents of a given linear index - implementation is not optimized
				exponent_type compute_exponent(size_type index) const { return base_type::compute_multi_index(index); }
				/// return the linear index of a given vector of monom exponents
				size_type compute_index(const exponent_type& exponent) const { return base_type::compute_index(exponent); }
				/// compute the basis at the given location to a memory pointed to by the given random access iterator
				template <class ta_iterator>
				void eval_basis(const source_type& location, ta_iterator iterator) const
				{
					cursor_type cursor;
					cgv::storage::collection::array<ta_iterator, ta_dimensionality> start_iterators = iterator;
					size_type n = get_size();
					*iterator = (source_value_type) 1;
					size_type degree = get_degree();
					for (; true; ) {
						// determine highest dimension, of which the exponent changes
						size_type highest_dimension = 0;
						size_type sum = cursor.multi_index[dimensionality-1];
						if (sum == degree)
							break;
						size_type dimension;
						for (dimension = dimensionality-2; ; --dimension) {
							sum += cursor.multi_index[dimension];
							if (sum == degree) {
								highest_dimension = dimension+1;
								break;
							}
							if (dimension == 0)
								break;
						}
						// step cursor and iterator
						step(cursor, iterator);
						// compute current basis value
						*iterator = (*(start_iterators[highest_dimension]))*location[highest_dimension];
						// update iterators
						for (dimension = 0; dimension <= highest_dimension; ++dimension)
							start_iterators[dimension] = iterator;
					}
				}
				/** compute the derivative of the basis with respect to the given dimension and put them in the order such that one can use the same 
				    coefficients for the derivative as for the function itself. The implementation cannot be used in place, i.e.
					basis and diff iterators must be different. */
				template <typename ta_basis_iterator, typename ta_diff_iterator>
				void diff_basis(ta_basis_iterator basis_iterator, size_type dimension, ta_diff_iterator diff_iterator) const
				{
					cursor_type cursor;
					size_type n = get_size();
					for (size_type i = 0; i < n; ++i) {
						if (cursor.multi_index[dimension] == 0)
							*diff_iterator = 0;
						else {
							multi_index_type multi_index = cursor.multi_index;
							--multi_index[dimension];
							index_type delta = compute_index(multi_index) - cursor.index;
							*diff_iterator = cursor.multi_index[dimension]* (*(basis_iterator + delta));
						}
						++basis_iterator;
						++diff_iterator;
						step(cursor);
					}
				}
				/// return the number of terms in the bases
				index_type get_size() const { return get_number_elements(); }
				/// return the degree
				size_type get_degree() const { return get_extent()-1; }
				/// set the degree
				void set_degree(size_type degree) { set_extent(degree+1); }				
				/// return the character used for the variable of the given dimension
				static char get_variable(size_type dimension)
				{
					static const char* variables = "xyzwuvst";
					return variables[dimension];
				}
				/// print the basis term to a stream
				void print(std::ostream& os, const exponent_type& exponent) const
				{
					bool printed_something = false;
					for (size_type dimension = 0; dimension < dimensionality; ++dimension)
						if (exponent[dimension] > 0) {
							if (printed_something)
								os << "*";
							else
								printed_something = true;
							os << get_variable(dimension);
							if (exponent[dimension] > 1)
								os << '^' << exponent[dimension];
						}
					if (!printed_something)
						os << '1';
				}
				/// print the basis term to a stream
				void print(std::ostream& os, size_type index) const
				{
					exponent_type exponent = compute_multi_index(index);
					print(os, exponent);
				}
			};
			/** specialization for one dimensional taylor basis.
				*/
			template <typename ta_source_value>
			class taylor_basis<ta_source_value,1>
			{
			public:
				typedef cgv::type::size_type size_type;
				/// type of a vector that holds the exponents of a combined monom
				static const size_type dimensionality = 1;
				typedef size_type exponent_type;
				typedef ta_source_value source_value_type;
				typedef ta_source_value source_type;
				///
				size_type degree;
				/// construct basis from given degree
				taylor_basis(size_type degree) { set_degree(degree); }
				/// compute the basis at the given location to a memory pointed to by the given random access iterator
				template <class ta_iterator>
				void eval_basis(const source_type& location, ta_iterator iterator) const
				{
					size_type n = get_size();
					*iterator = (source_value_type) 1;
					for (size_type i=1; i<n; ++i) {
						++iterator;
						*iterator *= location;
					}
				}
				/** compute the derivative of the basis and put them in the order such that one can use the same 
				    coefficients for the derivative as for the function itself. The implementation cannot be used in 
					place, i.e. basis and diff iterators must be different. */
				template <typename ta_basis_iterator, typename ta_diff_iterator>
				void diff_basis(ta_basis_iterator basis_iterator, ta_diff_iterator diff_iterator) const
				{
					size_type n = get_size();
					*diff_iterator = 0;
					++diff_iterator;
					for (size_type i = 1; i < n; ++i)
						*diff_iterator = i*(*basis_iterator);
					++basis_iterator;
					++diff_iterator;
				}
				/// return the number of terms in the bases
				size_type get_size() const { return degree+1; }
				/// return the degree
				size_type get_degree() const { return degree; }
				/// set the degree
				void set_degree(size_type _degree) { degree = _degree; }				
				/// print the basis term to a stream
				void print(std::ostream& os, size_type exponent) const
				{
					if (exponent == 0)
						os << '1';
					else {
						os << 'x';
						if (exponent > 1)
							os << '^' << exponent;
					}
				}
			};
		}
	}
}
