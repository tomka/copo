#pragma once

#include <cgv/type/reflect/show.h>
#include <cgv/type/ds/list_alg.h>
#include <cgv/type/ds/pair.h>
#include <cgv/type/ctrl/foreach.h>

namespace cgv {
	namespace type {
		namespace ds {
			namespace Set {
				/// this type is used to wrap all instances of a type set, where P defines the sorting predicate and T the contens of the set, the List::iterator wrapper struct is also used for sets
				template <typename P, typename T>
				struct set
				{
					typedef P predicate;
					typedef T contens;
				};
			}
			/// defines a type \c type with a type set of variable length, which is defined by the number of template arguments (a maximum of 8 arguments can be supplied)
			template <typename P, typename T1 = invalid_type, typename T2 = invalid_type, typename T3 = invalid_type, typename T4 = invalid_type,
								  typename T5 = invalid_type, typename T6 = invalid_type, typename T7 = invalid_type, typename T8 = invalid_type>
			struct set
			{
				typedef typename list<T1,T2,T3,T4,T5,T6,T7,T8>::type l;
				typedef typename remove_duplicates<l>::type ul;
				typedef typename sort<ul,P>::type s;
				typedef typename Set::set<P, typename contens<s>::type> type;
			};

			/// use the implementation of lists for stream output
			template <typename P, typename T>
			std::ostream& operator << (std::ostream& os, const reflect::show<Set::set<P,T> >&)
			{
				return os << reflect::show<List::list<T> >();
			}
			/// use the implementation of lists for empty
			template <typename P, typename T> struct empty  <Set::set<P,T> > : public empty<List::list<T> > {};
			/// use the implementation of lists for size
			template <typename P, typename T> struct size   <Set::set<P,T> > : public size<List::list<T> > {};
			/// use the implementation of lists for contens
			template <typename P, typename T> struct contens<Set::set<P,T> > { typedef T type; };
			/// use the list iterator also for sets
			template <typename P, typename T> struct begin  <Set::set<P,T> > : public begin<List::list<T> > {};
			/// use the list iterator also for sets
			template <typename P, typename T> struct end    <Set::set<P,T> > : public end<List::list<T> > {};
			/// use the implementation of lists for iter_at
			template <typename P, typename T, int I> struct iter_at<Set::set<P,T>, I> : public iter_at<List::list<T>, I> {};
			/// use the implementation of lists for at
			template <typename P, typename T, int I> struct at<Set::set<P,T>, I> : public at<List::list<T>, I> {};
			/// use the implementation of lists for contains
			template <typename P, typename T, typename X> struct contains<Set::set<P,T>, X> : public contains<List::list<T>, X> {};

			/// defines a type \c type with a type set resulting from adding X to S
			template <typename S, typename X>   struct add : public invalid_type {};
			/// defines a type \c type with a type set resulting from union of S1 and S2
			template <typename S1, typename S2> struct unite : public invalid_type {};
			/// defines a type \c type with a type set resulting from the intersection of S1 and S2
			template <typename S1, typename S2> struct intersect : public invalid_type {};

			template <typename P, typename T, typename X>
			struct add<Set::set<P,T>, X>
			{
				typedef Set::set<P,T> S;
				typedef typename ctrl::if_<typename contains<S, X>::type, S, 
					Set::set<P,typename contens<typename sorted_add<List::list<T>, X, P>::type>::type> >::type type;
			};
			template <typename P, typename T, typename X>
			struct remove<Set::set<P,T>, X>
			{
				typedef Set::set<P,typename contens<typename remove<List::list<T>,X>::type>::type> type;
			};
			template <typename P, typename T1, typename T2> 
			struct unite<Set::set<P,T1>, Set::set<P,T2> >
			{
				struct add_element
				{
					template <typename I, typename T>
					struct apply
					{
						typedef typename add<T,typename deref<I>::type>::type type;
					};
				};
				typedef typename ctrl::foreach<Set::set<P,T2>, add_element, Set::set<P,T1> >::type type;
			};
			template <typename P, typename T1, typename T2> 
			struct intersect<Set::set<P,T1>, Set::set<P,T2> >
			{
				struct intersect_elements
				{
					template <typename I, typename T>
					struct apply
					{
						typedef typename deref<I>::type X;
						typedef typename ctrl::if_<typename contains<typename T::second, X>::type,
							pair<typename add<typename T::first, X>::type, typename T::second>, T>::type type;
					};
				};
				typedef typename ctrl::foreach<Set::set<P,T2>, intersect_elements, pair<typename set<P>::type, Set::set<P,T1> > >::type::first type;
			};
		}
	}
}
