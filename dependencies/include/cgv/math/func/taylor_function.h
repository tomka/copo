#pragma once

#include <cgv/math/func/taylor_basis.h>
#include <cgv/algorithm/skp.h>
#include <cgv/storage/collection/array.h>
#include <cgv/storage/view/triangular_view.h>
#include <cgv/storage/collection/vector.h>

namespace cgv {
	namespace math {
		namespace func {
			/** implementation of a multi-dimensional taylor function. It uses the taylor basis and a vector 
			    to store the coefficients to the basis.
				*/
			template <typename ta_source_value, typename ta_target_type, unsigned int ta_source_dimensionality = 1>
			class taylor_function : public cgv::storage::collection::vector<ta_target_type>,
									public taylor_basis<ta_source_value, ta_source_dimensionality>
			{
			public:
				typedef taylor_basis<ta_source_value, ta_source_dimensionality> basis_type;
				typedef taylor_function<ta_source_value, ta_target_type, ta_source_dimensionality> this_type;
				typedef ta_source_value source_value_type;
				typedef typename cgv::storage::collection::array<source_value_type,ta_source_dimensionality> source_type;
				typedef ta_target_type target_type;
				typedef typename cgv::storage::collection::vector<source_value_type> basis_vector_type;
				typedef typename cgv::storage::collection::array<target_type,ta_source_dimensionality> gradient_type;
			public:
				/// construct basis from given degree
				taylor_function(unsigned int degree) : basis_type(degree) { resize(get_size()); }
				/// evaluate the function at the given location 
				target_type operator () (const source_type& location) const 
				{ 
					basis_vector_type basis_vector(get_size());
					eval_basis(location, basis_vector.begin());
					return skp((*this), basis_vector);
				}
				/// compute the derivative with respect to the given dimension
				target_type diff(const source_type& location, unsigned int dimension) const
				{
					basis_vector_type basis_vector(get_size());
					eval_basis(location, basis_vector.begin());
					basis_vector_type diff_vector(get_size());
					diff_basis(basis_vector.begin(), dimension, diff_vector.begin());
					return skp(diff_vector, (*this));
				}
				/// compute the gradient
				gradient_type grad(const source_type& location) const
				{
					gradient_type gradient;
					basis_vector_type basis_vector(get_size());
					eval_basis(location, basis_vector.begin());
					basis_vector_type diff_vector(get_size());
					for (size_type dimension=0; dimension<dimensionality; ++dimension) {
						diff_basis(basis_vector.begin(), dimension, diff_vector.begin());
						gradient[dimension] = skp((*this), diff_vector);
					}
					return gradient;
				}
				/// stream output operator
				friend std::ostream operator << (std::ostream& os, const this_type& tf)
				{
					size_type n = tf.get_size();
					bool printed_something = false;
					for (size_type i=0; i<n; ++i) {
						if (tf[i] != 0) {
							if (tf[i] > 0 && printed_something)
								os << '+';
							printed_something = true;
							os << tf[i];
							if (i != 0) {
								os << '*';
								tf.print(os, i);
							}
						}
					}
					return os;
				}
			};
			/** specialization for one dimensional source type.
				*/
			template <typename ta_source_value, typename ta_target_type>
			class taylor_function<ta_source_value,ta_target_type,1> : 
									public cgv::storage::collection::vector<ta_target_type>,
									public taylor_basis<ta_source_value,1>
			{
			public:
				typedef taylor_basis<ta_source_value,1> basis_type;
				typedef ta_source_value source_value_type;
				typedef ta_source_value source_type;
				typedef ta_target_type target_type;
				typedef typename cgv::storage::collection::vector<source_value_type> basis_vector_type;
			public:
				/// construct basis from given degree
				taylor_function(unsigned int degree) : basis_type(degree) { resize(get_size()); }
				/// evaluate the function at the given location 
				target_type operator () (const source_type& location) const 
				{ 
					basis_vector_type basis_vector(get_size());
					eval_basis(location, basis_vector.begin());
					return cgv::algorithm::skp((*this), basis_vector);
				}
				/// compute the derivative
				target_type diff(const source_type& location) const
				{
					basis_vector_type basis_vector(get_size());
					eval_basis(location, basis_vector.begin());
					basis_vector_type diff_vector(get_size());
					diff_basis(basis_vector.begin(), diff_vector.begin());
					return skp((*this), diff_vector);
				}
				/// stream output operator
				friend std::ostream operator << (std::ostream& os, const this_type& tf)
				{
					size_type n = tf.get_size();
					bool printed_something = false;
					for (size_type i=0; i<n; ++i) {
						if (tf[i] != 0) {
							if (tf[i] > 0 && printed_something)
								os << '+';
							printed_something = true;
							os << tf[i];
							if (i != 0) {
								os << '*';
								tf.print(os, i);
							}
						}
					}
					return os;
				}
			};
		}
	}
}
