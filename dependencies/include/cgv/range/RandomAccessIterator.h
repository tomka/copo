#pragma once

#include <cgv/defines/assert.h>
#include <cgv/type/index_type.h>
#include <cgv/type/func/clean.h>
#include <cgv/type/func/make_const.h>
#include <cgv/type/func/make_ref.h>
#include <cgv/type/LessThanComparable.h>
#include <cgv/range/BidirectionalIterator.h>

namespace cgv {
	namespace range {
		namespace RandomAccessIterator {
			/** a type is of the concept RandomAccessIterator if it implements the concepts BidirectionalIterator and
			    LessThanComparable, if it defines the type \c difference_type and if the following 
				expressions are valid and are convertible to the given type:
				\code
				T(a+i)
				T(a-i)
				T&(a+=i)
				T&(a-=i)
				difference_type(a-b)
				reference(a[i])
				const_reference(const a[i])
				\endcode
				 */
			struct tag : public cgv::type::concepts<cgv::type::LessThanComparable::tag, 
				BidirectionalIterator::tag>
			{
			};
			namespace Traits {
				/// allows to check if a type is a random access iterator and what the reference and difference types are
				template <typename T>
				struct traits : public cgv::type::not_specialized 
				{
					typedef typename cgv::type::cond::false_type implements;
				};
				/// implementation for tagged types
				template <typename T>
				struct traits<cgv::type::concept<tag,T> > : 
					public BidirectionalIterator::traits<cgv::type::concept<BidirectionalIterator::tag, T> >
				{
					typedef typename T::difference_type difference_type;
				};
				/// define all pointers to be random access iterators
				template <typename T> 
				struct traits<T*> : public BidirectionalIterator::traits<T*> 
				{
					typedef cgv::type::index_type difference_type;
				};
			}
			/// allows to check if a type is a random access iterator and what the reference and difference types are
			template <typename T>
			struct traits : public cgv::type::pick<Traits::traits, typename cgv::type::func::clean<T>::type>::type {};
			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				void constraints()
				{
					CGV_DEFINES_ASSERT_CONCEPT(cgv::type::LessThanComparable, T);
					CGV_DEFINES_ASSERT_CONCEPT(cgv::range::BidirectionalIterator, T);
					traits<T>::difference_type i = 5;
					T b = a+i;
					b = b-i;
					i = b-a;
					b += i;
					b -= i;
					traits<T>::reference ref = a[i];
				}
				T a;
			};
			/** derive your concept implementation from this tag in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(cgv::range::RandomAccessIterator, derived_class);
			};
			/// reference implementation of a RandomAccessIterator
			struct arche_type : public type::concepts<tag>, check<arche_type>
			{
				typedef int value_type;
				typedef int reference;
				typedef const int const_reference;
				typedef int difference_type;
				      reference operator *() { return i; }
				      reference operator ->() { return i; }
				const_reference operator *() const { return i; }
				const_reference operator ->() const { return i; }

				      reference operator [] (difference_type i)         { return i*i; }
				const_reference operator [] (difference_type i)   const { return i*i; }
				difference_type operator -  (const arche_type& t) const { return i-t.i; }
				arche_type      operator -  (difference_type i)   const { arche_type t(*this); t.i -= i; return t; }
				arche_type      operator +  (difference_type i)   const { arche_type t(*this); t.i += i; return t; }
				arche_type&     operator -= (difference_type _i)        { i -= _i; return *this; }
				arche_type&     operator += (difference_type _i)        { i += _i; return *this; }

				arche_type& operator ++() { ++i; return *this; }
				arche_type  operator ++(int i) { arche_type copy = *this; ++i; return copy; }
				arche_type& operator --() { --i; return *this; }
				arche_type  operator --(int i) { arche_type copy = *this; --i; return copy; }
				bool operator == (const arche_type& t) const { return i == t.i; }
				bool operator != (const arche_type& t) const { return i != t.i; }
				bool operator <  (const arche_type& t) const { return i <  t.i; }
				bool operator <= (const arche_type& t) const { return i <= t.i; }
				bool operator >  (const arche_type& t) const { return i >  t.i; }
				bool operator >= (const arche_type& t) const { return i >= t.i; }
				int i;
			};
		}
	}
}