#pragma once

#include <cgv/type/concept.h>
#include <cgv/type/cond/bool.h>
#include <cgv/type/cond/is_base_of.h>
#include <cgv/type/cond/is_fundamental.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/func/clean.h>

namespace cgv {
	namespace type {
		namespace CopyConstructible {
			/** a type is of the concept CopyConstructible if the expression <tt>T a(b);</tt> is valid. */
			struct tag
			{
			};
			namespace Traits {
				/// the standard implementation defines fundamental types to be copy constructible and all others not
				template <typename T>
				struct traits : public not_specialized 
				{
					typedef cond::is_fundamental<T> implements;
				};
				/// define all tagged classes to implement the copy constructible concept
				template <typename T>
				struct traits<concept<tag,T> >
				{
					typedef cond::true_type implements;
				};
				/// define all pointers to be copy constructible
				template <typename T> 
				struct traits<T*>
				{
					typedef cond::true_type implements;
				};
			}
			/// allows to check if a type is copy constructible or not via the \c implements type
			template <typename T>
			struct traits : public pick<Traits::traits,typename func::clean<T>::type>::type {};
			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				void constraints()
				{							
					T instance2(instance);
					// avoid warnings on unreferenced variables
					typename func::drop_ref<T>::type* p = &instance;
				}
				T instance;
			};
			/** derive your concept implementation from the check struct in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(cgv::type::CopyConstructible, derived_class);
			};
			/// reference implementation of a CopyConstructible
			class arche_type : public concepts<tag>, check<arche_type>
			{
			public:
				arche_type(const arche_type& copy) { }
			};
		}
	}
}