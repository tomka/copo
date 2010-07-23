#pragma once

#include <cgv/type/concept.h>
#include <cgv/type/cond/bool.h>
#include <cgv/type/cond/is_base_of.h>
#include <cgv/type/cond/is_fundamental.h>
#include <cgv/type/Dereferencible.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/func/drop_const.h>
#include <cgv/type/func/drop_ref.h>

namespace cgv {
	namespace type {
		namespace DefaultConstructible {
			/** a type is of the concept DefaultConstructible if the expression <tt>T a;</tt> is valid. */
			struct tag
			{
			};
			namespace Traits {
				/// the standard implementation defines fundamental types to be default constructible and all others not
				template <typename T>
				struct traits : public not_specialized 
				{
					typedef cond::is_fundamental<T> implements;
				};
				/// define all tagged classes to implement the default constructible concept
				template <typename T>
				struct traits<concept<tag,T> >
				{
					typedef cond::true_type implements;
				};
				/// define all pointers to be default constructible
				template <typename T> 
				struct traits<T*>
				{
					typedef cond::true_type implements;
				};
			}
			/// allows to check if a type is default constructible or not
			template <typename T>
			struct traits : public pick<Traits::traits,typename func::drop_const<T>::type>::type {};

			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				void constraints()
				{							
					T instance;
					// avoid warnings on unreferenced variables
					typename func::drop_ref<T>::type* p = &instance;
				}
			};
			/** derive your concept implementation from this tag in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(cgv::type::Dereferencible, derived_class);
			};
			/// reference implementation of a DefaultConstructible
			struct arche_type : public concepts<tag>, public check<arche_type> 
			{
				arche_type() {}
			};
		}
	}
}