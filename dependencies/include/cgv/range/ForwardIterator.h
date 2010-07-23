#pragma once

#include <cgv/defines/assert.h>
#include <cgv/type/cond/bool.h>
#include <cgv/type/concept.h>
#include <cgv/type/DefaultConstructible.h>
#include <cgv/type/CopyConstructible.h>
#include <cgv/type/Assignable.h>
#include <cgv/type/Dereferencible.h>
#include <cgv/type/EqualComparable.h>

namespace cgv {
	namespace range {
		namespace ForwardIterator {
			/** a type is of the concept ForwardIterator if it implements the concepts DefaultConstructible, 
			    CopyConstructible, Assignable, Dereferencible and EqualComparable and if the expressions 
				<tt>++a</tt> and <tt>a++</tt> are valid and if <tt>++a</tt> can be converted to type <tt>T&</tt>. */
			struct tag : public cgv::type::concepts<cgv::type::DefaultConstructible::tag, 
				cgv::type::CopyConstructible::tag, cgv::type::Assignable::tag, 
				cgv::type::Dereferencible::tag, cgv::type::EqualComparable::tag>
			{
			};
			namespace Traits {
				/// not specialized implementation 
				template <typename T>
				struct traits : public cgv::type::not_specialized 
				{
					typedef typename cgv::type::cond::false_type implements;
				};
				/// implementation for tagged types
				template <typename T>
				struct traits<cgv::type::concept<tag,T> > : 
					public cgv::type::Dereferencible::traits<cgv::type::concept<cgv::type::Dereferencible::tag, T> >
				{
				};
				/// implementation for pointers
				template <typename T> 
				struct traits<T*> : public cgv::type::Dereferencible::traits<T*> { };
			}
			/// allows to check if a type is a forward iterator and what the reference type is
			template <typename T>
			struct traits : public cgv::type::pick<Traits::traits, typename cgv::type::func::clean<T>::type>::type {};
			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				void constraints()
				{
					CGV_DEFINES_ASSERT_CONCEPT(cgv::type::CopyConstructible, T);
					CGV_DEFINES_ASSERT_CONCEPT(cgv::type::Assignable, T);
					CGV_DEFINES_ASSERT_CONCEPT(cgv::type::Dereferencible, T);
					CGV_DEFINES_ASSERT_CONCEPT(cgv::type::EqualComparable, T);
					T instance2 = ++instance;
					T instance3 = instance++;
				}
				T instance;
			};
			/** derive your concept implementation from this tag in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(cgv::range::ForwardIterator, derived_class);
			};
			/// reference implementation of a ForwardIterator
			struct arche_type : public cgv::type::concepts<tag>, public check<arche_type>
			{
				typedef int value_type;
				typedef int& reference;
				reference operator *() { return i; }
				reference operator ->() { return i; }
				arche_type& operator ++() { ++i; return *this; }
				arche_type  operator ++(int i) { arche_type copy = *this; ++i; return copy; }
				bool operator == (const arche_type& t) const { return i == t.i; }
				bool operator != (const arche_type& t) const { return i != t.i; }
				int i;
			};
		}
	}
}