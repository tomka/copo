#pragma once

#include <cgv/type/concept.h>
#include <cgv/type/ds/set.h>
#include <cgv/type/cond/is_base_of.h>
#include <cgv/type/reflect/consts.h>
#include <cgv/type/reflect/show.h>
#include <cgv/type/expr/expression.h>

namespace cgv {
	namespace type {
		namespace expr {
			/// implement a placeholder with the help of an int_const
			template <int i>
			struct placeholder : public reflect::int_const<i>
			{
			};
			/// declare the placeholders i,j,k,l
			static const placeholder<0> i = placeholder<0>();
			static const placeholder<1> j = placeholder<1>();
			static const placeholder<2> k = placeholder<2>();
			static const placeholder<3> l = placeholder<3>();
			/// declare the placeholders _1,_2,_3,_4
			static const placeholder<0> _1 = placeholder<0>();
			static const placeholder<1> _2 = placeholder<1>();
			static const placeholder<2> _3 = placeholder<2>();
			static const placeholder<3> _4 = placeholder<3>();

			template <int i>
			std::ostream& operator << (std::ostream& os, const reflect::show<placeholder<i> >&)
			{
				return os << "_" << i;
			}

			/// implement an expression for a placeholder PH
			template <typename PH> 
			struct placeholder_expression : public concepts<Expression::tag>, 
											public Expression::check<placeholder_expression<PH> >
			{
				typedef placeholder_expression<PH> this_type;
				// construct a set with the placeholder as sole element
				typedef typename ds::set<ph_sort_pred, PH>::type placeholder_set;
				// the placeholder expression cannot bind placeholders by itself
				template <typename PH1>
				struct placeholder_traits
				{
					typedef PH1 placeholder_type;
					static bool is_bindable()      { return false; }
					static bool unique_bindable()  { return false; }
					static bool binds_to_range()   { return false; }
				};
				// the placeholder expression can be 
				template <typename BI>
				struct binding_traits
				{
					typedef typename placeholder_binding<BI, PH>::is_bound is_bound;
					typedef typename placeholder_binding<BI, PH>::type argument_type;
					typedef typename ctrl::if_<is_bound, argument_type, this_type>::type return_type;
					typedef typename bind_struct<BI>::type binding;
					template <bool bound>
					static return_type select_execute(const binding& bs)
					{
						return bs.ref<select_placeholder<BI, PH>::pos>();
					}
					template <>
					static return_type select_execute<false>(const binding&)
					{
						return *this;
					}
					static return_type execute(const this_type&,  const binding& bs) 
					{
						return select_execute<is_bound::value>(bs);
					}
				};

				/** nice test, but too complicated for the simple placeholder expression

				template <typename T1>
				typename binding_traits<typename bind_info<this_type,T1>::type>::return_type 
					operator() (const T1& v1) 
				{ 
					typedef typename bind_info<this_type,T1>::type BI;
					typedef typename binding_traits<BI> traits;
					typedef typename bind_struct<BI>::type BS;
					typedef typename select_placeholder<BI, PH>::type SP;
					BS bs;
					SP::ref(bs) = v1;
					return traits::execute(*this, bs);
				}
				*/
				/// the simple form of the () operator
				template <typename T1>
				typename const T1& operator() (const T1& v1) 
				{ 
					return v1;
				}
			};

			template <int i>
			std::ostream& operator << (std::ostream& os, const reflect::show<placeholder_expression<placeholder<i> > >&)
			{
				return os << "{_" << i << "}";
			}

			namespace Expression {
				namespace Traits {
					/// overload the expression traits for the placeholder type and tell that the placeholder_expression should be used
					template <int i> 
					struct traits<placeholder<i> >
					{
						typedef cond::false_type implements;
						typedef placeholder_expression<placeholder<i> > expression_type;
					};
				}
			}
		}
	}
}
