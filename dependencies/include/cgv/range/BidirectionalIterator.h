#pragma once

#include <cgv/defines/assert.h>
#include <cgv/type/concept.h>
#include <cgv/type/func/clean.h>
#include <cgv/range/ForwardIterator.h>

namespace cgv {
	namespace range {
		namespace BidirectionalIterator {
			/** a type is of the concept BidirectionalIterator if it implements the concept ForwardIterator and 
				if the expressions <tt>--a</tt> and <tt>a--</tt> are valid and can be converted to type <tt>T&</tt>. */
			struct tag : public cgv::type::concepts<ForwardIterator::tag>
			{
			};
			namespace Traits {
				/// allows to check if a type is a bidirectional iterator and what the reference type is
				template <typename T>
				struct traits : public cgv::type::not_specialized 
				{
					typedef typename cgv::type::cond::false_type implements;
				};
				/// implementation for tagged types
				template <typename T>
				struct traits<cgv::type::concept<tag,T> > : 
					public ForwardIterator::traits<cgv::type::concept<ForwardIterator::tag, T> >
				{
				};
				/// define all pointers to be bidirectional iterators
				template <typename T> 
				struct traits<T*> : public ForwardIterator::traits<T*> {};
			}
			/// allows to check if a type is a bidirectional iterator and what the reference type is
			template <typename T>
			struct traits : public cgv::type::pick<Traits::traits, typename cgv::type::func::clean<T>::type>::type {};
			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				void constraints()
				{
					CGV_DEFINES_ASSERT_CONCEPT(cgv::range::ForwardIterator, T);
					--instance;
					instance--;
				}
				T instance;
			};
			/** derive your concept implementation from this tag in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(cgv::range::BidirectionalIterator, derived_class);
			};
			/// reference implementation of a BidirectionalIterator
			struct arche_type : public cgv::type::concepts<tag>, public check<arche_type>
			{
				typedef int value_type;
				typedef int reference;
				typedef const int const_reference;
				reference operator *() { return i; }
				reference operator ->() { return i; }
				const_reference operator *() const { return i; }
				const_reference operator ->() const { return i; }
				arche_type& operator ++() { ++i; return *this; }
				arche_type  operator ++(int i) { arche_type copy = *this; ++i; return copy; }
				arche_type& operator --() { --i; return *this; }
				arche_type  operator --(int i) { arche_type copy = *this; --i; return copy; }
				bool operator == (const arche_type& t) const { return i == t.i; }
				bool operator != (const arche_type& t) const { return i != t.i; }
				int i;
			};
		}
	}
}