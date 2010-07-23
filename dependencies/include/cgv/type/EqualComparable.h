#pragma once

#include <cgv/type/concept.h>
#include <cgv/type/cond/bool.h>
#include <cgv/type/cond/is_base_of.h>
#include <cgv/type/cond/is_fundamental.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/func/clean.h>

namespace cgv {
	namespace type {
		namespace EqualComparable {
			/** a type is of the concept EqualComparable if the expressions <tt>a == b</tt> and <tt>a != b</tt> are
			    valid and can be converted to type <tt>bool</tt>. */
			struct tag { };

			namespace Traits {
				/// the standard implementation defines fundamental types to be equal comparable and all others not
				template <typename T>
				struct traits : public not_specialized 
				{
					typedef cond::is_fundamental<T> implements;
				};
				/// all tagged types are equal comparable
				template <typename T>
				struct traits<concept<tag, T> >
				{
					typedef cond::true_type implements;
				};
				/// define all pointers to be equal comparable
				template <typename T> 
				struct traits<T*>
				{
					typedef cond::true_type implements;
				};
			}
			/// allows to check if a type is equal comparable or not
			template <typename T>
			struct traits : public pick<Traits::traits,typename func::clean<T>::type>::type {};
			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				void constraints()
				{							
					bool b = instance == instance;
					b     &= instance != instance;
				}
				T instance;
			};
			/** derive your concept implementation from the check struct in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(cgv::type::EqualComparable, derived_class);
			};
			/// reference implementation of a Dereferencible
			class arche_type : public concepts<tag>, check<arche_type>
			{
				int i;
			public:
				bool operator == (const arche_type& t) const { return i == t.i; }
				bool operator != (const arche_type& t) const { return i != t.i; }
			};
		}
	}
}