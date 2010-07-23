#pragma once

#include <cgv/type/index_type.h>
#include <cgv/range/expression/binary_expression.h>

namespace cgv {
	namespace range {
		namespace expression {

	/** the random access expression is used to access an element of a random access range via an expression that computes the index */
	template <typename T1, typename T2>
	struct random_access_expression : public binary_expression<T1, T2, random_access_expression<T1, T2> >
	{
		/// return a reference to the values pointed to by the first iterator
		typedef typename cgv::type::func::promote_const<T1, typename cgv::type::Dereferencible::traits<iterator_1>::reference>::type expression_return_type;
		/// define this type
		typedef typename random_access_expression<T1, T2> this_type;
		/// define base type
		typedef typename binary_expression<T1, T2, random_access_expression<T1, T2> > expression_base_type;
		/// perform const_cast to allow usage with const () operator
		random_access_expression(construct_argument_type_1 value_1, typename cgv::type::func::make_const<construct_argument_type_2>::type value_2) :
			expression_base_type(value_1, const_cast<construct_argument_type_2>(value_2)) {}
		///
		expression_return_type execute_at(const iterator& iter) const
		{
			// execute the expression that computes the index
			cgv::type::int32 index = (cgv::type::int32) (typename Expression::traits<T2>::execute_at(second, iter.second));
			// lookup the element pointed to by the index
			return first[index];
		}
	};
		}
	}
}
