#pragma once

namespace cgv {
	namespace math {
		namespace func {
			/** iterator that iterates */
			template <typename source_type, typename source_value_type, typename func_type>
			struct func_iterator
			{
				const func_type& func;
				source_type location;
				source_value_type delta;
				unsigned int source_dimension;
				func_iterator(const source_type& _loc, unsigned int _source_dimension, 
					          const source_value_type& _delta, const func_type& _func) : location(_loc), func(_func) {}
				target_type operator * () { return func(loc); }
				func_iterator operator ++() { location[source_dimension] += delta; }
				func_iterator operator --() { location[source_dimension] -= delta; }
				func_iterator operator + (int i) { location[source_dimension] += i*delta; }
				func_iterator operator - (int i) { location[source_dimension] -= i*delta; }
				int operator - (const func_iterator& fi) { return (location[source_dimension]-fi.location[source_dimension])/delta; }
			};
			/** use the given bidirectional iterator to compute the derivative with central differences */
			template <typename target_iterator_type, typename epsilon_type>
			target_iterator_type::value_type diff_central(target_iterator_type target_iterator, epsilon_type epsilon)
			{
				++target_iterator;
				target_iterator_type::value_type result = *target_iterator;
				target_iterator -= 2;
				result -= *target_iterator;
				result /= 2*epsilon;
				return result;
			}
			/** use the given bidirectional iterator to compute the derivative with forward differences */
			template <typename target_iterator_type, typename epsilon_type>
			target_iterator_type::value_type diff_forward(target_iterator_type target_iterator, epsilon_type epsilon)
			{
				++target_iterator;
				target_iterator_type::value_type result = *target_iterator;
				--target_iterator;
				result -= *target_iterator;
				result /= epsilon;
				return result;
			}
			/** use the given bidirectional iterator to compute the derivative with backward differences */
			template <typename target_iterator_type, typename epsilon_type>
			target_iterator_type::value_type diff_backward(target_iterator_type target_iterator, epsilon_type epsilon)
			{
				target_iterator_type::value_type result = *target_iterator;
				--target_iterator;
				result -= *target_iterator;
				result /= epsilon;
				return result;
			}
		}
	}
}