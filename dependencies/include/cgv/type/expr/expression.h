#pragma once

#include <cgv/type/concept.h>
#include <cgv/type/ds/list_alg.h>
#include <cgv/type/cond/is_less.h>
#include <cgv/type/reflect/consts.h>
#include <cgv/type/reflect/realize.h>

namespace cgv {
	namespace type {
		namespace expr {
			template <typename T> struct const_expression;
			/** concept for expressions*/
			namespace Expression {
				struct tag {};
				
				namespace Traits {
					/// the default traits implementation holds any value as constant
					template <typename T> 
					struct traits : public not_specialized
					{
						typedef cond::false_type implements;
						typedef const_expression<T> expression_type;
					};

					/// for tagged types the types of the expression are used
					template <typename T> 
					struct traits<concept<tag,T> >
					{
						typedef cond::true_type implements;
						typedef T expression_type;
						typedef T storage_type;
						typedef T binding_predicate;
						typedef T placeholder_predicate;
						typedef typename T::placeholder_set placeholder_set;
					};
				}
				/// provides information of how to build an expression from a type
				template <typename T>
				struct traits : public pick<Traits::traits,T>::type {};
				
				/// constraints necessary for concept checking
				template <typename T> 
				struct concept_check 
				{
					void constraints() 
					{ 
					}
				};
				template <class derived_class>
				struct check
				{
					CGV_DEFINES_ASSERT_CONCEPT(cgv::type::expr::Expression, derived_class);
				};
			}
			/// define the sorting predicate for the placeholder sets
			typedef ctrl::cond_pred2<cond::is_less> ph_sort_pred;
			/** this type function takes the placeholder set of expression E and a list of types
			    T1, T2, ... and returns a list with placeholder - type pairs, what is used to
				describe the placeholder binding for an expression. */
			template <typename E, typename T1, typename T2 = invalid_type, typename T3 = invalid_type, typename T4 = invalid_type,
				   typename T5 = invalid_type, typename T6 = invalid_type, typename T7 = invalid_type, typename T8 = invalid_type>
			struct bind_info
			{
				typedef typename ds::paired_list<typename E::placeholder_set, 
					typename ds::list<T1,T2,T3,T4,T5,T6,T7,T8>::type>::type type;
			};
			/** this type function takes a bind info and returns a type that can hold one instance for
			    each type to which the placeholders are bound */
			template <typename BI>
			struct bind_struct
			{
				typedef typename ds::split_paired_list<BI>::type::second L;
				typedef typename reflect::realize<L>::type type;
			};
			/** this is an extended template function that returns the type bound to a given 
			    placeholder in a bind_info and also defines the type is_bound, telling if the
				placeholder is bound at all. If not, the return type is invalid_type. */
			template <typename BI, typename PH>
			struct placeholder_binding
			{
				template <typename X>
				struct find_pair_first
				{
					template <typename P>
					struct apply
					{
						typedef cond::is_same<typename P::first, X> type;
					};
				};
				typedef typename ds::find_first<BI,find_pair_first<PH> >::type I;
				typedef cond::not<cond::is_same<I, typename ds::end<BI>::type> > is_bound;
				typedef typename ds::deref<I>::type::second type;
			};
			/** type function that returns for a bind_info and a placeholder a type that has a static member 
			    function ref(bind_struct<BI>::type), which returns a reference to the realization of the
				placeholders value in the bind_struct of BI. Use only in the case when PH is really an
				element of BI!! */
			template <typename BI, typename PH>
			struct select_placeholder
			{
				typedef typename bind_struct<BI>::L L;
				static const int pos = typename ds::pos<BI, typename placeholder_binding<BI,PH>::I>::type::value;
//				typedef typename ds::iter_at<L, p>::type I;
//				typedef typename reflect::deref_realization<L, I> type;
			};
		}
	}
}