#pragma once

#include <cgv/utils/pair.h>
#include <cgv/type/concept.h>
#include <cgv/type/ds/list_alg.h>
#include <cgv/type/cond/is_less.h>
#include <cgv/type/cond/and.h>
#include <cgv/type/reflect/consts.h>
#include <cgv/type/reflect/realize.h>
#include <cgv/type/func/promote.h>
#include <cgv/type/expr/binary_expression.h>

namespace cgv {
	namespace type {
		namespace expr {
			template <typename E1, typename E2> 
			struct add_expression : public binary_expression<E1,E2,add_expression<E1,E2> >
			{
				typedef add_expression<E1,E2> this_type;
				typedef binary_expression<E1,E2,this_type> base_type;
				///
				template <typename BI>
				struct binding_traits : public base_type::binding_traits<BI>
				{
					typedef typename base_type::binding_traits<BI>::is_bound is_bound;
					typedef typename base_type::binding_traits<BI>::return_type return_type;
					typedef typename base_type::binding_traits<BI>::BS BS;
					template <bool bound>
					static return_type select_execute(const this_type& ex, const BS& bs)
					{
						return BT1::execute(ex.first, bs) + BT2::execute(ex.second, bs);
					}
					template <>
					static return_type select_execute<false>(const this_type& ex, const BS& bs)
					{
						return *this;
					}
					static return_type execute(const this_type& ex,  const BS& bs) 
					{
						return select_execute<is_bound::value>(ex, bs);
					}
				};
				/// construct from two expressions
				add_expression(construct_argument_type_1 e1, construct_argument_type_2 e2) : 
					base_type(e1,e2)
				{
				}
			};
			namespace Expression
			{
				/// provide add operator for expressions
				template <typename T1, typename T2>
				add_expression<T1, T2> operator + (const T1& value_1, const T2& value_2)
				{
					typedef typename Expression::traits<T1>::expression_type E1;
					typedef typename Expression::traits<T1>::expression_type E2;
					return expression::add_expression<E1, E2>(value_1,value_2);
				}
			}
		}
	}
}