#pragma once

#include <cgv/defines/assert.h>
#include <cgv/type/ds/list.h>
#include <cgv/type/reflect/consts.h>
#include <cgv/type/func/drop_ref.h>
#include <cgv/type/func/make_ref.h>
#include <cgv/type/ctrl/foreach.h>
#include <cgv/utils/pair.h>

namespace cgv {
	namespace type {
		namespace ds {
			/// defines the type \c type with a type sequence where type X was prepend, if it was not already contained in the sequence
			template <typename S, typename X>             struct append_if_not_contained
			{
				typedef typename ctrl::if_<typename contains<S,X>::type, S, typename append<S,X>::type>::type type;
			};
			/// defines the type \c type with a type sequence where type X was prepend, if it was not already contained in the sequence
			template <typename S, typename X>             struct prepend_if_not_contained
			{
				typedef typename ctrl::if_<typename contains<S,X>::type, S, typename prepend<S,X>::type>::type type;
			};
			/// compute an int constant with the position of iterator I2 relative to I1
			template <typename I1, typename I2>
			struct pos_from
			{
				static const int p = typename pos_from<typename next<I1>::type, I2>::type::value+1;
				typedef reflect::int_const<p> type;
			};
			/// compute an int constant with the position of iterator I2 relative to I1
			template <typename I>
			struct pos_from<I,I>
			{
				typedef reflect::int_const<0> type;
			};
			/// compute an int constant with the position of iterator I in sequence S relative to begin<S>::type
			template <typename S, typename I>
			struct pos
			{
				typedef typename pos_from<typename begin<S>::type, I>::type type;
			};
			/// sort the sequence T according to predicate P
			template <typename T, typename P>             struct sort     : public invalid_type {};
			/// remove all duplicates from the sequence
			template <typename T>                         struct remove_duplicates : public invalid_type {};

			/// construct from two sequences S1, S2 a list of pairs with size<S1> elements pair<at<S1,i>,at<S2,i> >
			template <typename S1, typename S2>
			struct paired_list
			{
				struct pair_elements
				{
					template <typename I1, typename T>
					struct apply
					{
						typedef typename ds::deref<I1>::type X1;
						typedef typename T::first I2;
						typedef typename ds::deref<I2>::type X2;
						typedef typename ds::next<I2>::type T_first;
						typedef typename ds::append<typename T::second, ds::pair<X1,X2> >::type T_second;
						typedef ds::pair<T_first,T_second> type;
					};
				};
				/// construct a list of pairs of placeholder - argument types
				typedef typename ctrl::foreach<S1, pair_elements, 
					pair<typename ds::begin<S2>::type, ds::empty_list> >::type::second type;
			};
			/// construct from a sequence S of type pairs a pair of two lists by splitting each pair into two elements
			template <typename S>
			struct split_paired_list
			{
				struct split_paired_elements
				{
					template <typename I, typename T>
					struct apply
					{
						typedef typename ds::deref<I>::type P;
						typedef typename P::first X1;
						typedef typename P::second X2;
						typedef typename T::first L1;
						typedef typename T::second L2;
						typedef typename ctrl::if_<cond::is_same<X1,invalid_type>,
							L1, typename ds::append<L1, X1>::type>::type N1;
						typedef typename ctrl::if_<cond::is_same<X2,invalid_type>,
							L2, typename ds::append<L2, X2>::type>::type N2;
						typedef ds::pair<N1,N2> type;
					};
				};
				/// construct a list of pairs of placeholder - argument types
				typedef typename ctrl::foreach<S, split_paired_elements, 
					ds::pair<ds::empty_list,ds::empty_list> >::type type;
			};
			/// this class should be specialized with a list of types and is derived from all the types in the list
			template <typename L> struct base_chain : public invalid_type {};
			/// recursive definition
			template <typename T>
			struct base_chain<ds::List::list<T> > : public ds::head<ds::List::list<T> >::type,
				public base_chain<typename ds::tail<ds::List::list<T> >::type>
			{};
			/// standard end of recursion
			template <typename X>
			struct base_chain<ds::List::list<ds::pair<X,ds::List::empty_list> > > : public X
			{
			};
			/// emergency end of recursion only if template is instanciated with empty list
			template <>
			struct base_chain<ds::empty_list> {};

			/// prepend type X at the beginning of L if X is not a base class of any type in L
			template <typename L, typename X>
			struct prepend_if_not_base
			{
				struct X_is_base
				{
					template <typename T>
					struct apply
					{
						typedef cond::is_base_of<X,T> type;
					};
				};
				typedef typename ctrl::if_<cond::is_same<typename ds::find_first<L,X_is_base>::type,
					typename ds::end<L>::type>, typename ds::prepend<L,X>::type, L>::type type;
			};

			/// defines a predicate that compares two types by its value member
			struct smaller_value
			{
				template <typename T1, typename T2>
				struct apply
				{
					static const bool less = T1::value < T2::value;
					typedef cgv::type::cond::bool_type<less> type;
				};
			};
			/// defines a predicate that compares two types by its size
			struct smaller_size
			{
				template <typename T1, typename T2>
				struct apply
				{
					static const bool less = sizeof(T1)<sizeof(T2);
					typedef cgv::type::cond::bool_type<less> type;
				};
			};

			template <typename T, typename P>
			struct sort<List::list<T>, P>
			{
				template <typename P>
				struct build_sorted
				{
					template <typename I, typename T>
					struct apply
					{
						typedef typename sorted_add<T, typename deref<I>::type, P>::type type;
					};
				};
				typedef typename ctrl::foreach<typename List::list<T>, build_sorted<P>, list<>::type >::type type;
			};
			template <typename T>
			struct remove_duplicates<List::list<T> >
			{
				struct build_without_duplicates
				{
					template <typename I, typename T>
					struct apply
					{
						typedef typename deref<I>::type X;
						typedef typename ctrl::if_<typename contains<T, X>::type, T, typename append<T,X>::type>::type type;
					};
				};
				typedef typename ctrl::foreach<typename List::list<T>, build_without_duplicates, list<>::type >::type type;
			};
		}
	}
}
