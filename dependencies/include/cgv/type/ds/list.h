#pragma once

#include <cgv/type/reflect/show.h>
#include <cgv/type/invalid_type.h>
#include <cgv/type/cond/bool.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/cond/is_same.h>
#include <cgv/type/ctrl/apply.h>
#include <cgv/type/ds/pair.h>

namespace cgv {
	namespace type {
		namespace ds {
			namespace List {
				/// this type is used to wrap all instances of a type list
				template <typename T>
				struct list
				{
				};
				/// this type is used to wrap all instances of iterators of a type list
				template <typename T> 
				struct iterator
				{
				};
				/// type to represent the empty list
				struct empty_list
				{
				};
				/// constructor for a type list with one type
				template <typename T1>
				struct list1 { typedef pair<T1,empty_list> type; };
				/// constructor for a type list with two types
				template <typename T1, typename T2>
				struct list2 { typedef pair<T1,typename list1<T2>::type> type; };
				/// constructor for a type list with three types
				template <typename T1, typename T2, typename T3>
				struct list3 { typedef pair<T1,typename list2<T2,T3>::type> type; };
				/// constructor for a type list with four types
				template <typename T1, typename T2, typename T3, typename T4> 
				struct list4 { typedef pair<T1,typename list3<T2,T3,T4>::type> type; };
				/// constructor for a type list with five types
				template <typename T1, typename T2, typename T3, typename T4,typename T5> 
				struct list5 { typedef pair<T1,typename list4<T2,T3,T4,T5>::type> type; };
				/// constructor for a type list with six types
				template <typename T1, typename T2, typename T3, typename T4,typename T5, typename T6> 
				struct list6 { typedef pair<T1,typename list5<T2,T3,T4,T5,T6>::type> type; };
				/// constructor for a type list with seven types
				template <typename T1, typename T2, typename T3, typename T4,typename T5, typename T6, typename T7> 
				struct list7 { typedef pair<T1,typename list6<T2,T3,T4,T5,T6,T7>::type> type; };
				/// constructor for a type list with eight types
				template <typename T1, typename T2, typename T3, typename T4,typename T5, typename T6, typename T7, typename T8> 
				struct list8 { typedef pair<T1,typename list7<T2,T3,T4,T5,T6,T7,T8>::type> type; };
			}
			/// defines a type \c type with a list of variable length, which is defined by the number of template arguments (a maximum of 8 arguments can be supplied)
			template <typename T1 = invalid_type, typename T2 = invalid_type, typename T3 = invalid_type, typename T4 = invalid_type,
					  typename T5 = invalid_type, typename T6 = invalid_type, typename T7 = invalid_type, typename T8 = invalid_type>
			struct list
			{
				typedef
					typename ctrl::if_<cond::is_same<T8,invalid_type>,
						typename ctrl::if_<cond::is_same<T7,invalid_type>,
							typename ctrl::if_<cond::is_same<T6,invalid_type>,
								typename ctrl::if_<cond::is_same<T5,invalid_type>,
									typename ctrl::if_<cond::is_same<T4,invalid_type>,
										typename ctrl::if_<cond::is_same<T3,invalid_type>,
											typename ctrl::if_<cond::is_same<T2,invalid_type>,
												typename ctrl::if_<cond::is_same<T1,invalid_type>,
													List::list<List::empty_list>,
													List::list<typename List::list1<T1>::type>
												>::type,
												List::list<typename List::list2<T1,T2>::type>
											>::type,
											List::list<typename List::list3<T1,T2,T3>::type>
										>::type,
										List::list<typename List::list4<T1,T2,T3,T4>::type>
									>::type,
									List::list<typename List::list5<T1,T2,T3,T4,T5>::type>
								>::type,
								List::list<typename List::list6<T1,T2,T3,T4,T5,T6>::type>
							>::type,
							List::list<typename List::list7<T1,T2,T3,T4,T5,T6,T7>::type>
						>::type,
						List::list<typename List::list8<T1,T2,T3,T4,T5,T6,T7,T8>::type>
					>::type type;
			};

			/// defines the type corresponding to the empty list, which should be used to terminate recursive templates
			typedef List::list<List::empty_list> empty_list;
			/// defines the type corresponding to the end iterator, which should be used to terminate recursive templates
			typedef List::iterator<List::empty_list> end_iter;

			inline std::ostream& operator << (std::ostream& os, const reflect::show<empty_list>&)
			{
				return os << "{}";
			}
			/// implement the streaming of list types via the show wrapper
			template <typename T>
			inline std::ostream& operator << (std::ostream& os, const reflect::show<List::list<T> >&)
			{
				typedef List::list<T> L;
				if (cond::is_same<typename tail<L>::type, empty_list>::value)
					return os << reflect::show<typename head<L>::type>();
				else
					return os << reflect::show<typename head<L>::type>() << ", " << reflect::show<typename tail<L>::type>();
			}


			/// is derived from a boolean type that tells, whether a sequence is empty
			template <typename S> struct empty   : public invalid_type {};
			/// defines a static const int value with the number of elements in a sequence
			template <typename S> struct size    : public invalid_type {};
			/// defines the type \c type with the internal representation of the sequence
			template <typename S> struct contens : public invalid_type {};
			/// defines the type \c type with the head type of a list
			template <typename T> struct head    : public invalid_type {};
			/// defines the type \c type with the tail as type list
			template <typename T> struct tail    : public invalid_type {};
			/// defines the type \c type with the iterator pointing to the first element of a sequence
			template <typename S> struct begin   : public invalid_type {};
			/// defines the type \c type with the iterator pointing after the last element of a sequence
			template <typename S> struct end     : public invalid_type {};
			/// defines the type \c type with the next iterator pointing after iterator I
			template <typename I> struct next    : public invalid_type {};
			/// defines the type \c type with the type to which the iterator I points or the invalid type, if I points after the end of a sequence
			template <typename I> struct deref   : public invalid_type {};
			/// defines the type \c type of the iterator pointing to the element at position P in a sequence
			template <typename S, int P>                  struct iter_at  : public invalid_type {};
			/// defines the type \c type of the element at position P in a sequence
			template <typename S, int P>                  struct at       : public invalid_type {};
			/// defines the type \c type with a type sequence where type X was inserted at the position pointed to by the iterator I
			template <typename S, typename I, typename X> struct insert   : public invalid_type {};
			/// defines the type \c type with a type sequence where type X was append
			template <typename S, typename X>             struct append   : public invalid_type {};
			/// defines the type \c type with a type sequence where type X was prepend
			template <typename S, typename X>             struct prepend  : public invalid_type {};
			/// defines the type \c type with a type sequence where the element at the position pointed to by iterator I was deleted
			template <typename S, typename I>             struct clear   : public invalid_type {};
			/// defines the type \c type with a type sequence where the first appearance of element X was deleted
			template <typename S, typename X>             struct remove  : public invalid_type {};
			/// defines a boolean type \c type that tells if the sequence contains type X
			template <typename S, typename X>             struct contains : public invalid_type {};
			/// transform the sequence S by replacing each contained type X with Trans<X>::type
			template <typename S, template <typename _X> class Trans> struct transform : public invalid_type {};
			/// search X in a sequence S starting at the given sequence iterator and return the iterator I with deref<I>::type == X or end<S> iterator if X was not found in S
			template <typename I, typename X>             struct find_from : public invalid_type {};
			/// search X in a sequence S and return the iterator I with deref<I>::type == X or end<S> iterator if X was not found in S
			template <typename S, typename X>             struct find : public invalid_type {};
			/// search the first iterator position, where in a sequence S starting at iterator I the predicate is fulfilled the first time, i.e. where P::apply<deref<I>::type>, or end<S> iterator if predicate is not fulfilled at all
			template <typename I, typename P> struct find_first_from : public invalid_type {};
			/// search the first iterator position, where the predicate is fulfilled the first time, i.e. where P::apply<deref<I>::type>, or end<S> iterator if predicate is not fulfilled at all
			template <typename S, typename P> struct find_first : public invalid_type {};
			/// search the first iterator position, where in a sequence S starting at iterator I the predicate is fulfilled the first time, i.e. where P::apply<X,deref<I>::type>, or end<S> iterator if predicate is not fulfilled at all
			template <typename I, typename X, typename P> struct find_first_from2 : public invalid_type {};
			/// search the first iterator position, where the predicate is fulfilled the first time, i.e. where P::apply<X,deref<I>::type>, or end<S> iterator if predicate is not fulfilled at all
			template <typename S, typename X, typename P> struct find_first2 : public invalid_type {};
			/// defines the type \c type with a type sequence where the element X has been inserted at the first position in sequence S, where the predicate P is fulfilled
			template <typename S, typename X, typename P> struct sorted_add : public invalid_type {};

			template <typename T>
			struct empty<List::list<T> > : public cond::is_same<List::empty_list, T> {};
			
			template <typename T>
			struct size<List::list<T> >
			{
				typedef List::list<T> L;
				static const int value = size<typename tail<L>::type>::value + 1;
			};
			template <>
			struct size<empty_list>
			{
				static const int value = 0;
			};

			template <typename T> 
			struct contens<List::list<T> >
			{
				typedef T type;
			};

			template <typename T> 
			struct head<List::list<T> >
			{
				typedef typename T::first type;
			};
			template <> 
			struct head<empty_list>
			{
				typedef invalid_type type;
			};

			template <typename T> 
			struct tail<List::list<T> >
			{
				typedef List::list<typename T::second> type;
			};
			template <> 
			struct tail<empty_list>
			{
				typedef empty_list type;
			};

			template <typename T>
			struct begin<List::list<T> >
			{
				typedef List::iterator<T> type;
			};

			template <typename T>
			struct end<List::list<T> >
			{
				typedef end_iter type;
			};
			template <typename T>
			struct next<List::iterator<T> >                { typedef List::iterator<typename T::second> type; };
			template <> 
			struct next<end_iter> { typedef end_iter type; };
			
			template <typename T> struct deref<List::iterator<T> >                { typedef typename T::first type; };
			template <>           struct deref<end_iter > { typedef invalid_type type; };

			template <typename T, int P>
			struct iter_at<List::list<T>, P>
			{
				struct next_expression 
				{ 
					template <typename T> struct apply { typedef typename next<T>::type type; }; 
				};
				typedef typename ctrl::apply<
					next_expression, 
					typename begin<List::list<T> >::type, 
					P
				>::type type;
			};
			template <typename T, int P>
			struct at<List::list<T>, P>
			{
				typedef typename deref<typename iter_at<List::list<T>,P>::type>::type type;
			};
			template <typename T, typename I, typename X>
			struct insert<List::list<T>, List::iterator<I>, X>
			{
				typedef typename List::list<
					pair<
						typename T::first,
						typename contens<typename insert<List::list<typename T::second>, List::iterator<I>, X>::type>::type
					>
				> type;
			};
			template <typename T, typename X>
			struct insert<List::list<T>, List::iterator<T>, X>
			{
				typedef typename List::list<pair<X,T> > type;
			};
			template <typename T, typename X>
			struct append<List::list<T>, X>
			{
				typedef List::list<T> L;
				typedef typename insert<L,typename end<L>::type,X>::type type;
			};
			template <typename T, typename X>
			struct prepend<List::list<T>, X>
			{
				typedef List::list<T> L;
				typedef typename insert<L,typename begin<L>::type,X>::type type;
			};
			namespace List
			{
				template <typename L, typename I>
				struct clear
				{
					typedef typename List::clear<typename tail<L>::type,I>::type shortend_tail;
					typedef typename ctrl::if_<cond::is_same<typename begin<L>::type,I>,
						typename tail<L>::type,
						typename prepend<shortend_tail, typename head<L>::type>::type
					>::type type;
				};
				template <typename I>
				struct clear<List::list<List::empty_list>, I>
				{
					typedef List::list<List::empty_list> type;
				};
			}
			template <typename T, typename I>
			struct clear<List::list<T>, List::iterator<I> >
			{
				typedef typename List::clear<List::list<T>,List::iterator<I> >::type type;
			};
			template <typename T, typename X>
			struct remove<List::list<T>, X>
			{
				typedef typename clear<List::list<T>, typename find<List::list<T>,X>::type>::type type;
			};
			template <typename T, typename X> 
			struct contains<List::list<T>, X>
			{
				typedef List::list<T> L;
				typedef typename ctrl::if_<
					cond::is_same<X,typename head<L>::type>,
					cond::true_type,
					typename contains<typename tail<L>::type, X>::type
				>::type type;
			};
			template <typename X> 
			struct contains<empty_list, X>
			{
				typedef cond::false_type type;
			};
			template <typename T, template <typename _X> class Trans>
			struct transform<List::list<T>, Trans>
			{
				typedef typename List::list<T> L;
				typedef List::list<pair<typename Trans<typename head<L>::type>::type, 
					typename contens<typename transform<typename tail<L>::type, Trans>::type>::type> > type;
			};
			template <template <typename _X> class Trans>
			struct transform<empty_list, Trans>
			{
				typedef empty_list type;
			};
			template <typename T, typename X> 
			struct find_from<List::iterator<T>,X>
			{
				typedef List::iterator<T> I;
				typedef typename ctrl::if_<cond::is_same<typename deref<I>::type,X>,
					I,
					typename find_from<typename next<I>::type, X>::type
				>::type type;
			};
			template <typename X> 
			struct find_from<end_iter,X>
			{
				typedef typename end_iter type;
			};

			template <typename T, typename X> 
			struct find<List::list<T>,X>
			{
				typedef typename find_from<typename begin<List::list<T> >::type, X>::type type;
			};

			template <typename T, typename P> 
			struct find_first_from<List::iterator<T>,P>
			{
				typedef List::iterator<T> I;
				typedef typename ctrl::if_<typename P::apply<typename deref<I>::type>::type,
					I, typename find_first_from<typename next<I>::type, P>::type
				>::type type;
			};
			template <typename P> 
			struct find_first_from<end_iter,P>
			{
				typedef typename end_iter type;
			};

			template <typename T, typename P> 
			struct find_first<List::list<T>,P>
			{
				typedef typename find_first_from<typename begin<List::list<T> >::type, P>::type type;
			};
			
			template <typename T, typename X, typename P> 
			struct find_first_from2<List::iterator<T>,X,P>
			{
				typedef List::iterator<T> I;
				typedef typename ctrl::if_<typename P::apply<X, typename deref<I>::type>::type,
					I,
					typename find_first_from2<typename next<I>::type, X, P>::type
				>::type type;
			};
			template <typename X, typename P> 
			struct find_first_from2<end_iter,X,P>
			{
				typedef typename end_iter type;
			};

			template <typename T, typename X, typename P> 
			struct find_first2<List::list<T>,X,P>
			{
				typedef typename find_first_from2<typename begin<List::list<T> >::type, X, P>::type type;
			};

			template <typename T, typename X, typename P> 
			struct sorted_add<List::list<T>,X,P>
			{
				typedef List::list<T> L;
				typedef typename insert<L, typename find_first2<L,X,P>::type, X>::type type;
			};
		}
	}
}
