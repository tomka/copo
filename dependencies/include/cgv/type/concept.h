#pragma once

#include <cgv/type/invalid_type.h>
#include <cgv/type/ds/list.h>
#include <cgv/type/ds/list_alg.h>
#include <cgv/type/cond/is_base_of.h>
#include <cgv/type/cond/is_same.h>
#include <cgv/type/cond/not.h>
#include <cgv/type/cond/or.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/not_specialized.h>

namespace cgv {
	namespace type {
		/// dummy class to mark structs or classes that implement at least one concept and have a typedef named concept_list containing a list of concept tags
		struct implements_concept {};
		/// condition to check if a class X is marked to be a concept implementation
		template <typename X>
		struct is_concept_implementation : public cond::is_base_of<implements_concept,X>
		{};
		/// return a type list with all concepts, that class X implements. If no concepts are implemented, the list is empty
		template <typename X>
		struct base_concept_list
		{
			template <bool is_concept_implementation>
			struct select_base_concept_list        { typedef typename X::concept_list type; };
			template <>
			struct select_base_concept_list<false> { typedef ds::empty_list type; };
			typedef typename select_base_concept_list<is_concept_implementation<X>::value>::type type;
		};
		/** derive your class that implements one or several concepts C1 ... Cn from 
		    concepts<C1::tag,...,Cn::tag>, what automatically derives from all concept tags and
			implements_concept and also defines the type \c concept_list that contains all the 
			passed concept tags */
		template <typename T1, typename T2 = invalid_type, typename T3 = invalid_type, typename T4 = invalid_type,
		          typename T5 = invalid_type, typename T6 = invalid_type, typename T7 = invalid_type, typename T8 = invalid_type>
		struct concepts : public ds::base_chain<typename ds::prepend_if_not_base<
							typename ds::list<T1,T2,T3,T4,T5,T6,T7,T8>::type,
							implements_concept>::type>
		{
			typedef typename ds::list<T1,T2,T3,T4,T5,T6,T7,T8>::type concept_list;
		};

		/// condition to check if the traits template T is specialized for X. This only works if the not specialized version is derived from not_specialized
		template <template <typename _X> class T, typename X>
		struct is_specialized : public 
			ctrl::if_<cond::is_base_of<not_specialized, T<X> >,
				cond::false_type,
				cond::true_type
			>::type
		{ };
		/** wrapper struct used to specialize a traits template T for a concept C. For this
			specialize template <typename X> struct T<concept<C::tag,X> >. X will then always
			by an implementation of concept C. */
		template <typename ConceptTag, typename ConceptImplementation>
		struct concept {};

		/** wrapper struct used to hint a preferred concept.
		    The first template argument T is the traits template. The second
			template argument is a type X that implements the preferred concept
			and probably further concepts. The preferred concept is specified 
			inside the traits_hint specialization through a typedef on the tag 
			of the preferred concept. The match template will then prefer to use
			for type X the hinted preferred concept, if the traits template has a
			specialization for this concept and not for the type X itself. 
		*/
		template <template <typename _T> class T, typename X>
		struct traits_hint { typename invalid_type type; };
		/** traits template that allows to hint a prefered concept with the
		    traits_hint template. For a traits template T and a class X the
			hint template is specialized for traits_hint<T,X>. This specialization
			must not be derive from not_specialized and must define the type 
			\c type with the concept tag of the preferred concept. */
		template <typename T>
		struct hint : public not_specialized
		{
			typedef invalid_type prefer;
		};

		/// this namespace holds templates that are used by the pick template
		namespace Pick {
			/** search in the concept list CL a concept tag for which the traits template T
			    is specialized. Define in type is_found, whether a specialization was found
				and in type \c type the traits specialization. */
			template <template <typename _X> class T, typename CL, typename X>
			struct find_specialization_in_concept_list
			{
				/// only construct T<concept<deref<I>,X> > if iterator I does not point to end
				template <bool is_found, typename I>
				struct deref_iter_if { typedef T<concept<typename ds::deref<I>::type,X> > type; };
				template <typename I>
				struct deref_iter_if<false,I> { typedef T<X> type; };
				// define a predicate that checks if the given traits template is specialized for a concept tag in the concept list
				struct T_is_specialized
				{
					template <typename ConceptTag>
					struct apply
					{
						typedef is_specialized<T,concept<ConceptTag,X> > type;
					};
				};
				// find the iterator of the first concept for which the traits template has been specialized
				typedef typename ds::find_first<CL,T_is_specialized>::type I;
				/// declare boolean type that tells if a concept implementation was found
				typedef cond::not<cond::is_same<I,typename ds::end<CL>::type> > is_found;
				// call pick by iterator, with a bool that tells, whether it is the end iterator
				typedef typename deref_iter_if<is_found::value,I>::type type;
			};
			/** Construct the list of base concepts from the concept list CL_CURRENT and select 
				only these, which are not contained in CL_UNION nor in CL_CURRENT. If the new
				concept list is empty, return T<X>, otherwise do recursion for the first concept
				that T implements. */
			template <template <typename _X> class T, typename CL_UNION, typename CL_CURRENT, typename X>
			struct by_concept_base_list
			{
				/// construct the next level of the base concept hierarchy
				struct add_new_base_concept_tags
				{
					template <typename I, typename L>
					struct apply
					{
						struct prepend_if_new
						{
							template <typename I, typename L>
							struct apply
							{
								typedef typename ds::deref<I>::type X;
								typedef typename ctrl::if_<cond::or<typename ds::contains<CL_UNION,X>::type,
									                                typename ds::contains<CL_CURRENT,X>::type>,
									L, typename ds::prepend_if_not_contained<L,X>::type>::type type;
							};
						};
						typedef typename base_concept_list<typename ds::deref<I>::type>::type BCL;
						typedef typename ctrl::foreach<BCL, prepend_if_new, L>::type type;
					};
				};

				typedef typename ctrl::foreach<CL_CURRENT, add_new_base_concept_tags, ds::empty_list>::type CL_NEW;
				/// merge the current level with all previous levels
				struct prepend_predicate
				{
					template <typename I, typename S>
					struct apply
					{
						typedef typename ds::prepend_if_not_contained<S, typename ds::deref<I>::type>::type type;
					};
				};
				typedef typename ctrl::foreach<CL_CURRENT, prepend_predicate, CL_UNION>::type CL_UNION_NEW;


				/** if no further base concept is found, just return T<X>, otherwise use 
				    by_concept_base_list_if_found, parameterized over the bool telling whether 
					the new base concept level contains a specialization of T */

				/// if a specialization found, return this specialization
				template <bool is_found>
				struct select
				{
					typedef typename find_specialization_in_concept_list<T,CL_CURRENT,X>::type type;
				};
				/// otherwise recursively search the next level
				template <>
				struct select<false> 
				{
					typedef typename by_concept_base_list<T,CL_UNION_NEW,CL_NEW,X>::type type;
				};
				typedef typename select<find_specialization_in_concept_list<T,CL_CURRENT,X>::is_found::value>::type type; 
			};
			template <template <typename _X> class T, typename CL_UNION, typename X>
			struct by_concept_base_list<T,CL_UNION,ds::empty_list,X>
			{
				typedef T<X> type;
			};
			/// 
			template <template <typename _X> class T, typename X>
			struct check_for_concepts
			{
				// pass a bool argument to the pick by_concept_list, that tells, whether X implements at least one concept
				typedef typename by_concept_base_list<T, ds::empty_list, typename base_concept_list<X>::type, X>::type type;
			};

			template <bool specialized, template <typename _X> class T, typename X>
			struct by_hint
			{
				typedef T<concept<typename hint<traits_hint<T, X> >::type, X> > type;
			};
			template <template <typename _X> class T, typename X>
			struct by_hint<false,T,X>
			{
				// if the traits template is not specialized for the hinted tag, call the checker for the next priority: the concepts implemented by X
				typedef typename check_for_concepts<X,T>::type type;
			};
			template <bool specialized, template <typename _X> class T, typename X>
			struct check_for_hint
			{
				// check if the traits class is specialized for the hinted concept tag
				static const bool specialized = is_specialized<T, concept<typename hint<traits_hint<T, X> >::type, X> >::value;
				// and pass this bool as argument to the pick by_hint template
				typedef typename by_hint<specialized,T,X>::type type;
			};
			template <template <typename _X> class T, typename X>
			struct check_for_hint<false,T,X>
			{
				// if no hint is given, check if X implements one or more concepts which can be picked in the next priority
				typedef typename check_for_concepts<T,X>::type type;
			};
		}
		/** the pick template replaces for a given traits template T and a type X the expression T<X> by
		    a more flexible selection process. It will define \c type to a specialization of T in the
			following order
			-# pick T<X> if specialized
			-# pick T<concept<hint<traits_hint<T,X> >::type,X> > if specialized and hint<traits_hint<T,X> > is specialized
			-# pick T<concept<min{i,at<X::concept_list,i>::type},X> > if specialized, X is derived from implements_concept and 
			   X::concept_list contains a specialized concept		
			-# in all other cases T<X> is picked
			
			For this to work, the standard implementation of T has to be derived from not_specialized.
		*/
		template <template <typename _X> class T, typename X>
		struct pick
		{
			// is the traits template is specialized for X, this implementation has highest priority
			typedef typename ctrl::if_<is_specialized<T, X>, T<X>,
				// otherwise pick by the next priority, which are the hints; pass as bool argument to the checker, whether the traits_hint has been specialized for the queried combination T<X>
				typename Pick::check_for_hint<is_specialized<hint, traits_hint<T, X> >::value,T,X>::type >::type type;
		};
	}
}
