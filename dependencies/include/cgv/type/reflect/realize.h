#ifndef CGV_DEFINES_LOOP_ENTERED
#ifndef CGV_TYPE_REFLECT_REALIZE_H

#include <cgv/type/ds/list.h>
#include <cgv/type/func/clean.h>
#include <cgv/type/ctrl/foreach.h>
#include <cgv/type/cond/is_convertible_to.h>
#include <cgv/type/cond/is_const.h>
#include <cgv/type/func/make_ref.h>
#include <cgv/type/func/make_const.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/utils/pair.h>

namespace cgv {
	namespace type {
		namespace reflect {
			/** defines a type \c type that is the transformation of a type sequence S 
			    to data structure composed of pairs. If S is empty \c invalid_type is 
				returned, if S contains only one element, the type of this element is 
				returned */
			template <typename S>                         struct realize : public invalid_type {};
			/** same as realize but first transforms the sequence of types to a
			    sequence without const nor reference modifiers. */
			template <typename S>                         struct realize_cleaned : public invalid_type {};
			/// defines a struct as type \c type with the types value_type and return_type and a static method return_type ref(realize<S>::type& r), which returns a reference to the member in the realization that was generated for the type to which the iterator I points in the sequence S
			template <typename S, typename I>             struct deref_realization : public invalid_type {};

			template <typename T>
			struct realize<ds::List::list<T> >
			{
				typedef ds::List::list<T> L;
				struct concat_pairs
				{
					template <typename I, typename T>
					struct apply
					{
						typedef typename ds::deref<I>::type X;
						typedef typename ctrl::if_<cond::is_same<T,invalid_type>, X, cgv::utils::pair<X,T> >::type type;
					};
				};
				template <typename T>
				struct get_arg_type
				{
					typedef const typename cgv::type::func::clean<T>::type& type;
				};
				template <typename T>
				struct get_arg_type<T&>
				{
					typedef T& type;
				};
				/// the realization has a member of recursively nested pairs. The entries can be accessed with the ref and set methods
				typedef typename ctrl::foreach_reverse<L, concat_pairs, invalid_type>::type pair_list;
				struct type
				{
					pair_list p_l;
					/// construct with standard initialization of list entry realizations
					type() 
					{
					}
					/// construct and initialize realization of first list entry
					type(typename get_arg_type<typename ds::at<L,0>::type>::type v0)
						: p_l(v0) { }
//for (i = 1; i < 7; ++i) {
#define LOOP_I_BEGIN 1
#define LOOP_I_FILE <cgv/type/reflect/realize.h>
#include <cgv/defines/loop_i.h>
#endif
#endif
#if LOOP_NESTING_LEVEL == 1
	// std::cout << "type(";
	type (
	// for (j = 0; j < i+1; ++j) {
#define LOOP_J_BEGIN 0
#define LOOP_J_END I+1
		// if (j > 0)
		//		std::cout << ", " << std::endl << "     ";
#define LOOP_J_SEP ,
		// std::cout << "typename get_arg_type<typename ds::at<L," << j << ">::type>::type v" << j;
#define LOOP_J_BODY typename get_arg_type<typename ds::at<L,J>::type>::type JOIN(v,J)
	// }
#include <cgv/defines/loop_j.h>
	// std::cout << ") : p_l(v0";
	) : p_l(v0
	// for (j = 1; j < i; ++j) {
#define LOOP_J_BEGIN 1
#define LOOP_J_END I
#define LOOP_J_FILE <cgv/type/reflect/realize.h>
#define CGV_TYPE_REFLECT_REALIZE_LOOP_1
#include <cgv/defines/loop_j.h>
#undef CGV_TYPE_REFLECT_REALIZE_LOOP_1
#endif
#if (LOOP_NESTING_LEVEL == 2) && defined(CGV_TYPE_REFLECT_REALIZE_LOOP_1)
		// for (k = j; k < i; ++k) {
#define LOOP_K_BEGIN J
#define LOOP_K_END I
		// std::cout << "," << std::endl << "     utils::pair<typename ds::at<L," << k << ">::type";
#define LOOP_K_BODY , utils::pair<typename ds::at<L,K>::type
		// }
#include <cgv/defines/loop_k.h>
		// std::cout << ", typename ds::at<L," << i << ">::type";
		, typename ds::at<L,I>::type
		// for (k = j; k < i; ++k) {
#define LOOP_K_BEGIN J
#define LOOP_K_END I
		//	if (j > k)
		//		std::cout << " ";
		//	std::cout << ">";
#define LOOP_K_BODY >
		//}
#include <cgv/defines/loop_k.h>
		// std::cout << "(v" << j << std::endl;
		(JOIN(v,J)
	//}
#endif
#if LOOP_NESTING_LEVEL == 1
	// std::cout << ", v" << i;
	, JOIN(v,I)
	// for (j = 1; j < i+1; ++j)
#define LOOP_J_BEGIN 1
#define LOOP_J_END I+1
	//	std::cout << " )";
#define LOOP_J_BODY )
#include <cgv/defines/loop_j.h>
	// std::cout << " {}" << std::endl;
	{}
#endif
#ifndef CGV_DEFINES_LOOP_ENTERED
#ifndef CGV_TYPE_REFLECT_REALIZE_H

					template <int i, bool convertible, typename X>
					struct set_to
					{
						static bool call(type* t, X value)
						{
							t->ref<i>() = (ds::at<L,i>::type) value;
							return true;
						}
					};
					template <int i, typename X>
					struct set_to<i,false,X>
					{
						static bool call(type* t, X value)
						{
							return false;
						}
					};
					/// set the i-th parameter to the given value of type X, where i is given at compile time as template parameter. Return if i was in the valid range and X convertible to the realization of the ith list entry.
					template <int i, typename X>
					bool set(X value)
					{
						return set_to<i, cond::is_convertible_to<X,ds::at<L,i>::type>::value,X>::call(this,value);
					}
					/// set the i-th parameter to the given value of type X, where i is given at runtime time. Only values of i up to 9 are supported. Return if i was in the valid range and X convertible to the realization of the ith list entry.
					template <typename X>
					bool set(int i, X value)
					{
						if (i < 0 || i >= ds::size<L>::value)
							return false;
						switch (i) {
						case 0 : return set<0>(value); break;
						case 1 : return set<1>(value); break;
						case 2 : return set<2>(value); break;
						case 3 : return set<3>(value); break;
						case 4 : return set<4>(value); break;
						case 5 : return set<5>(value); break;
						case 6 : return set<6>(value); break;
						case 7 : return set<7>(value); break;
						}
						std::cerr << "realize::type::set(i,v) only implemented up to i == 7!!!" << std::endl;
						return false;
					}
					/// return a refererence the the realization of the ith list entry
					template <int i>
					typename func::make_ref<typename ds::at<L,i>::type>::type ref()
					{
						return deref_realization<L,typename ds::iter_at<L,i>::type>::ref(p_l);
					}
					/// return a const refererence the the realization of the ith list entry
					template <int i>
					typename func::make_ref<typename func::make_const<typename ds::at<L,i>::type>::type>::type ref() const
					{
						return deref_realization<L,typename ds::iter_at<L,i>::type>::ref(p_l);
					}
				};
			};

			template <typename T>
			struct realize_cleaned<ds::List::list<T> >
			{
				typedef typename ds::List::list<T> L;
				typedef typename ds::transform<L, func::clean>::type CL;
				typedef typename realize<CL>::type type;
			};

			template <typename T, typename I, bool one_method_only>
			struct deref_realization_switch
			{
				typedef ds::List::list<T> L;
				typedef typename func::drop_ref<typename ds::deref<I>::type>::type value_type;
				typedef typename func::make_ref<value_type>::type return_type;
				typedef typename func::make_const<return_type>::type const_return_type;
				typedef typename realize<L>::pair_list R;
				/// recursive implementation uses second entry in pair and performs recursion
				template <typename L, typename I>
				struct ref_impl_recursive
				{
					static return_type ref(typename cgv::type::func::make_ref<R>::type r)
					{ 
						return deref_realization<typename ds::tail<L>::type,I>::ref(r.second);
					}
					static const_return_type ref(typename cgv::type::func::make_const<typename cgv::type::func::make_ref<R>::type>::type r)
					{ 
						return deref_realization<typename ds::tail<L>::type,I>::ref(r.second);
					}
				};
				/// implementation where I is begin<L>::type and simply the first entry in the pair can be used
				template <typename L, typename I>
				struct ref_impl_begin
				{
					static return_type ref(typename cgv::type::func::make_ref<R>::type r)
					{ 
						return r.first;
					}
					static const_return_type ref(typename cgv::type::func::make_const<typename cgv::type::func::make_ref<R>::type>::type r)
					{ 
						return r.first;
					}
				};
				/// implementation where I points to the unique element in a list with one element only. Here the realization itself is returned
				template <typename L, typename I>
				struct ref_impl_single
				{
					static return_type ref(typename cgv::type::func::make_ref<R>::type r)
					{ 
						return r;
					}
					static const_return_type ref(typename cgv::type::func::make_const<typename cgv::type::func::make_ref<R>::type>::type r)
					{ 
						return r;
					}
				};
				/// second switch can assume that there are at least two elements in the list. Standard implementation calls recursive implementation
				template <typename L, typename I>
				struct ref_switch_2 
				{
					typedef ref_impl_recursive<L,I> type; 
				};
				/// specialize for the case, when I points to the beginning of L
				template <typename T>
				struct ref_switch_2<ds::List::list<T>, ds::List::iterator<T> > 
				{
					typedef ref_impl_begin<ds::List::list<T>,ds::List::iterator<T> > type; 
				};
				/// standard implementation uses second switch
				/// third switch can assume that there is exactly one element in the list. Standard implementation generates error
				template <typename L, typename I>
				struct ref_switch_3
				{
					struct DEREF_REALIZATION_WITH_ITERATOR_NOT_POINTING_TO_SINGLE_ELEMENT {};
					typedef DEREF_REALIZATION_WITH_ITERATOR_NOT_POINTING_TO_SINGLE_ELEMENT type; 
				};
				/// specialize for the case, when I points to the beginning of the single element list L
				template <typename T>
				struct ref_switch_3<ds::List::list<T>, ds::List::iterator<T> > 
				{
					typedef ref_impl_single<ds::List::list<T>,ds::List::iterator<T> > type; 
				};
				template <typename L, typename I>
				struct ref_switch
				{
					typedef typename ref_switch_2<L,I>::type type;
				};
				/// specialization for single element lists checks in ref_switch_3 if the iterator points to this element
				template <typename X, typename I>
				struct ref_switch<ds::List::list<ds::pair<X,ds::List::empty_list> >, I>
				{
					typedef ds::List::list<ds::pair<X,ds::List::empty_list> > L;
					typedef typename ref_switch_3<L,I>::type type;
				};
				/// specialization for empty list
				template <typename I>
				struct ref_switch<ds::empty_list, I>
				{
					struct DEREF_REALIZATION_ON_EMPTY_LIST {};
					typedef DEREF_REALIZATION_ON_EMPTY_LIST type;
				};
				static return_type ref(typename cgv::type::func::make_ref<R>::type r)
				{
					typedef typename ref_switch<L,I>::type selected_ref_impl;
					return selected_ref_impl::ref(r);
				}
				static const_return_type ref(typename cgv::type::func::make_const<typename cgv::type::func::make_ref<R>::type>::type r)
				{
					typedef typename ref_switch<L,I>::type selected_ref_impl;
					return selected_ref_impl::ref(r);
				}
			};


			template <typename T, typename I>
			struct deref_realization_switch<T,I,true>
			{
				typedef ds::List::list<T> L;
				typedef typename func::drop_ref<typename ds::deref<I>::type>::type value_type;
				typedef typename func::make_ref<value_type>::type const_return_type;
				typedef typename realize<L>::pair_list R;
				/// recursive implementation uses second entry in pair and performs recursion
				template <typename L, typename I>
				struct ref_impl_recursive
				{
					static const_return_type ref(typename cgv::type::func::make_const<typename cgv::type::func::make_ref<R>::type>::type r)
					{ 
						return deref_realization<typename ds::tail<L>::type,I>::ref(r.second);
					}
				};
				/// implementation where I is begin<L>::type and simply the first entry in the pair can be used
				template <typename L, typename I>
				struct ref_impl_begin
				{
					static const_return_type ref(typename cgv::type::func::make_const<typename cgv::type::func::make_ref<R>::type>::type r)
					{ 
						return r.first;
					}
				};
				/// implementation where I points to the unique element in a list with one element only. Here the realization itself is returned
				template <typename L, typename I>
				struct ref_impl_single
				{
					static const_return_type ref(typename cgv::type::func::make_const<typename cgv::type::func::make_ref<R>::type>::type r)
					{ 
						return r;
					}
				};
				/// second switch can assume that there are at least two elements in the list. Standard implementation calls recursive implementation
				template <typename L, typename I>
				struct ref_switch_2 
				{
					typedef ref_impl_recursive<L,I> type; 
				};
				/// specialize for the case, when I points to the beginning of L
				template <typename T>
				struct ref_switch_2<ds::List::list<T>, ds::List::iterator<T> > 
				{
					typedef ref_impl_begin<ds::List::list<T>,ds::List::iterator<T> > type; 
				};
				/// standard implementation uses second switch
				/// third switch can assume that there is exactly one element in the list. Standard implementation generates error
				template <typename L, typename I>
				struct ref_switch_3
				{
					struct DEREF_REALIZATION_WITH_ITERATOR_NOT_POINTING_TO_SINGLE_ELEMENT {};
					typedef DEREF_REALIZATION_WITH_ITERATOR_NOT_POINTING_TO_SINGLE_ELEMENT type; 
				};
				/// specialize for the case, when I points to the beginning of the single element list L
				template <typename T>
				struct ref_switch_3<ds::List::list<T>, ds::List::iterator<T> > 
				{
					typedef ref_impl_single<ds::List::list<T>,ds::List::iterator<T> > type; 
				};
				template <typename L, typename I>
				struct ref_switch
				{
					typedef typename ref_switch_2<L,I>::type type;
				};
				/// specialization for single element lists checks in ref_switch_3 if the iterator points to this element
				template <typename X, typename I>
				struct ref_switch<ds::List::list<ds::pair<X,ds::List::empty_list> >, I>
				{
					typedef ds::List::list<ds::pair<X,ds::List::empty_list> > L;
					typedef typename ref_switch_3<L,I>::type type;
				};
				/// specialization for empty list
				template <typename I>
				struct ref_switch<ds::empty_list, I>
				{
					struct DEREF_REALIZATION_ON_EMPTY_LIST {};
					typedef DEREF_REALIZATION_ON_EMPTY_LIST type;
				};
				static const_return_type ref(typename cgv::type::func::make_const<typename cgv::type::func::make_ref<R>::type>::type r)
				{
					typedef typename ref_switch<L,I>::type selected_ref_impl;
					return selected_ref_impl::ref(r);
				}
			};

			template <typename T, typename I>
			struct deref_realization<ds::List::list<T>,I> : 
				public deref_realization_switch<T, I, 
				ds::size<ds::List::list<T> >::value == 1 &&
				cgv::type::cond::is_const<typename ds::at<ds::List::list<T>,0>::type>::value>
			{
			};
		}
	}
}
#define CGV_TYPE_REFLECT_REALIZE_H
#endif
#endif