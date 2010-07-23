#pragma once

#include <cgv/defines/assert.h>
#include <cgv/type/concept.h>
#include <cgv/type/cond/bool.h>
#include <cgv/type/cond/is_base_of.h>
#include <cgv/type/cond/is_fundamental.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/func/drop_ref.h>

namespace cgv {
	namespace type {
		namespace Assignable {
			/** a type is of the concept Assignable if the expression <tt>a1 = a2</tt> is valid for non \c const and
			    non \c const references \c a1. */
			struct tag
			{
			};
			namespace Traits {
				/// default implemetation tells that all fundamental types are assignable
				template <typename T>
				struct traits: public not_specialized 
				{
					/// declare that all tagged types and all fundamental types implement the concept \c Assignable
					typedef cond::is_fundamental<T> implements;
				};
				/// implementation for tagged classes
				template <typename T>
				struct traits<concept<tag,T> >
				{
					typedef cond::true_type implements;
				};
				/// all pointers are assignable
				template <typename T>
				struct traits<T*> 
				{
					typedef cond::true_type implements;
				};
				/// all const types are not assignable
				template <typename T>
				struct traits<const T> 
				{
					typedef cond::false_type implements;
				};
			}
			/// allows to check if a type is assignable or not via the \c implements type
			template <typename T>
			struct traits : public pick<Traits::traits,typename func::drop_ref<T>::type>::type {};
			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				void constraints()
				{							
					instance = instance;
				}
				T instance;
			};
			/** derive your concept implementation from the check struct in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(cgv::type::Assignable, derived_class);
			};
			/// reference implementation of an Assignable
			class arche_type : public concepts<tag>, check<arche_type>
			{
			public:
				arche_type& operator = (const arche_type& copy) { return *this; }
			};
		}
	}
}
