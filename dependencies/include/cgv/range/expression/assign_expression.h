#pragma once

#include <cgv/type/Dereferencible.h>
#include <cgv/type/func/promote_const.h>
#include <cgv/range/expression/binary_expression.h>

namespace cgv {
	namespace range {
		namespace expression {

	/** the assign_list is used to assign a comma separated list of values */
	template <typename iterator_type>
	struct assign_list
	{
		iterator_type iterator;
		assign_list(iterator_type _iterator) : iterator(_iterator) {}
		/// comma assignment
		template <typename type>
		assign_list<iterator_type> operator , (const type& value)
		{
			++iterator;
			*iterator = typename cgv::type::Dereferencible::traits<iterator_type>::value_type(value);
			return *this;
		}
	};

	/** the assign expression delays assignments of ranges and allows for assignment of a comma separated list of values */
	template <typename T1, typename T2>
	struct assign_expression : public binary_expression<T1, T2, assign_expression<T1, T2> >
	{
		/// return a reference to the values pointed to by the first iterator
		typedef typename cgv::type::func::promote_const<T1, typename cgv::type::Dereferencible::traits<iterator_1>::reference>::type expression_return_type;
		/// define base type
		typedef typename binary_expression<T1, T2, assign_expression<T1, T2> > expression_base_type;
		/// define type of left side of assign for conversion operator
		typedef typename cgv::type::Dereferencible::traits<iterator_1>::value_type l_value_type;
		/// perform const_cast to allow usage with assign operator
		assign_expression(construct_argument_type_1 value_1, typename cgv::type::func::make_const<construct_argument_type_2>::type value_2) :
			expression_base_type(value_1, const_cast<construct_argument_type_2>(value_2)) {}
		///
		expression_return_type execute_at(const iterator& iter) const
		{
			// execute the right hand side expression 
			typename cgv::type::func::clean<return_type_2>::type value_2 = 
				typename Expression::traits<T2>::execute_at(second, iter.second);
			// convert the result to the type of the target range
			return *(iter.first) = (l_value_type) value_2;
		}
		/// comma assignment initiates list assignment
		template <typename type>
		assign_list<iterator_1> operator , (const type& value)
		{
			// perform assign on first entry
			iterator i = begin();
			execute_at(i);
			// step to second and assign the passed value
			++i.first;
			*(i.first) = (l_value_type) value;
			// deactivate this expression to avoid the execution of the execute method
			deactivate();
			// return a list assigner
			return assign_list<iterator_1>(i.first);
		}
	};
		}
	}
}
