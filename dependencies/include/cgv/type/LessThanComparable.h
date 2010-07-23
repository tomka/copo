#pragma once

#include <cgv/type/concept.h>
#include <cgv/type/cond/bool.h>
#include <cgv/type/cond/is_base_of.h>
#include <cgv/type/cond/is_fundamental.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/func/clean.h>
#include <cgv/type/EqualComparable.h>

namespace cgv {
	namespace type {
		namespace LessThanComparable {
			/** a type is of the concept LessThanComparable if the expressions <tt>a [<|<=|>|>=] b</tt> are
			    valid and can be converted to type <tt>bool</tt>. */
			struct tag : public concepts<cgv::type::EqualComparable::tag>
			{
			};
			namespace Traits {
				/// the standard implementation defines fundamental types to be less than comparable and all others not
				template <typename T>
				struct traits : public not_specialized  
				{
					typedef cond::is_fundamental<T> implements;
				};
				/// all tagged types are less than comparable
				template <typename T>
				struct traits<concept<tag, T> >
				{
					typedef cond::true_type implements;
				};
				/// define all pointers to be less than comparable
				template <typename T> 
				struct traits<T*>
				{
					typedef cond::true_type implements;
				};
			}
			/// allows to check if a type is less than comparable or not
			template <typename T>
			struct traits : public pick<Traits::traits,typename func::clean<T>::type>::type {};
			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				void constraints()
				{
					CGV_DEFINES_ASSERT_CONCEPT(cgv::type::EqualComparable, T);
					bool b = instance < instance;
					b &= instance <= instance;
					b &= instance > instance;
					b &= instance >= instance;
				}
				T instance;
			};
			/** derive your concept implementation from the check struct in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(cgv::type::LessThanComparable, derived_class);
			};
			/// reference implementation of a LessThanComparable
			class arche_type : public concepts<tag>, public check<arche_type>
			{
				int i;
			public:
				bool operator == (const arche_type& t) const { return i == t.i; }
				bool operator != (const arche_type& t) const { return i != t.i; }
				bool operator <  (const arche_type& t) const { return i < t.i; }
				bool operator <= (const arche_type& t) const { return i <= t.i; }
				bool operator >  (const arche_type& t) const { return i > t.i; }
				bool operator >= (const arche_type& t) const { return i >= t.i; }
			};
		}
	}
}