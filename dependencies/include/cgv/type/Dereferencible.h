#pragma once

#include <xutility>
#include <cgv/type/concept.h>
#include <cgv/type/cond/bool.h>
#include <cgv/type/cond/is_base_of.h>
#include <cgv/type/cond/is_const.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/func/make_const.h>
#include <cgv/type/func/make_ref.h>

namespace cgv {
	namespace type {
		namespace Dereferencible {
			/** a type is of the concept Dereferencible if it defines the types \c value_type and \c reference 
				and if the expressions <tt>*a</tt> and <tt>a-></tt> yield a result of 
				type \c reference. */
			struct tag { };
			namespace Traits {
				template <typename T>
				struct std_traits : public not_specialized
				{
					typedef cond::false_type implements;
				};
				template <typename T>
				struct std_iterator_traits : public not_specialized
				{
					typedef cond::true_type implements;
					typedef typename T::value_type value_type;					
					typedef typename T::reference reference;					
				};
				/// by default no type is dereferencible
				template <typename T>
				struct traits : public cgv::type::ctrl::if_<
					cgv::type::cond::is_base_of<std::_Iterator_base,T>,
					std_iterator_traits<T>, std_traits<T> >::type {};
				/// implementation for tagged dereferencibles
				template <typename T>
				struct traits<concept<tag,T> >
				{
					typedef cond::true_type implements;
					typedef typename T::value_type value_type;					
					typedef typename T::reference reference;					
				};
				/// specialization for pointer types
				template <typename T>
				struct traits<T*>
				{
					typedef cond::true_type implements;
					typedef       T  value_type;
					typedef       T& reference;
				};
			}
			/// allows to check if a type is dereferencible or not and provides access to the \c reference and \c const_reference type
			template <typename T>
			struct traits : public pick<Traits::traits,T>::type {};

			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				void constraints()
				{							
					traits<T>::reference ref = *instance;
					traits<T>::reference ref2 = instance.operator ->();
					traits<T>::reference ref3 = v;
				}
				T instance;
				typename traits<T>::value_type v;
			};
			/// special implementation for pointers, whose -> operator is not so easy to check
			template <typename T>
			struct concept_check<T*>
			{
				void constraints()
				{							
					traits<T*>::reference ref = *instance;
				}
				T* instance;
			};
			/** derive your concept implementation from this tag in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(cgv::type::Dereferencible, derived_class);
			};
			/// reference implementation of a Dereferencible
			class arche_type : public concepts<tag>, check<arche_type>
			{
				int i;
			public:
				typedef int  value_type;
				typedef int& reference;
				typedef const int& const_reference;
				reference operator * () { return i; }
				const_reference operator * () const { return i; }
				reference operator ->() { return i; }
				const_reference operator ->() const { return i; }
			};
		}
	}
}