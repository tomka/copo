#pragma once

#include <cgv/range/BidirectionalRange.h>

namespace cgv {
	namespace range {
		namespace RandomAccessRange {
			/** a type is of the concept RandomAccessRange if it implements the concept BidirectionRange where the iterator
			    types all implement the concept RandomAccessIterator, if it defines 
			    two types reference and const_reference and if the following expressions are valid and can be converted to 
				the indicated type. 
				\code
				reference(a[size_type])
				const_reference((const a)[size_type])
				\endcode
				*/
			struct tag : public type::concepts<BidirectionalRange::tag> { };

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
					public BidirectionalRange::traits<type::concept<BidirectionalRange::tag, T> >
				{
					typedef typename T::reference reference;
					typedef typename T::const_reference const_reference;
					/// the promote_const_reference type yields the const_reference for type T with const modifier and the reference type otherwise
					typedef typename cgv::type::ctrl::if_<cgv::type::cond::is_const<T>,const_reference,reference>::type promote_const_reference;
				};
			}
			/** allows to check if a type is a random access range and what the reference types are. In addition
			    it inherits the functionality of cgv::range::BidirectionalRange::traits and defines the type 
				\c promote_const_reference, which is equal to \c reference if the template parameter
				has no \c const modifier and equal to \c const_reference if it has. */
			template <typename T>
			struct traits : public cgv::type::pick<Traits::traits, typename cgv::type::func::drop_ref<T>::type>::type {};
			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				typedef typename type::func::make_const<typename type::func::make_ref<T>::type>::type T_const;
				void constraints()
				{
					CGV_DEFINES_ASSERT_CONCEPT(cgv::range::BidirectionalRange, T);
					CGV_DEFINES_ASSERT_CONCEPT(cgv::range::RandomAccessIterator, traits<T>::iterator);
					CGV_DEFINES_ASSERT_CONCEPT(cgv::range::RandomAccessIterator, traits<T>::const_iterator);
					CGV_DEFINES_ASSERT_CONCEPT(cgv::range::RandomAccessIterator, traits<T>::reverse_iterator);
					CGV_DEFINES_ASSERT_CONCEPT(cgv::range::RandomAccessIterator, traits<T>::const_reverse_iterator);
					traits<T>::reference ref = a[0];
					a[1] = ref;
					traits<T>::const_reference cref = c[0];
				}
				T a;
				T_const c;
			};
			/** derive your concept implementation from this tag in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(cgv::range::RandomAccessRange, derived_class);
			};
			/// reference implementation of a RandomAccessRange
			class arche_type : public type::concepts<tag>, public check<arche_type>
			{
				int c[4];
			public:
				typedef int* iterator;
				typedef const int* const_iterator;
				typedef int* reverse_iterator;
				typedef const int* const_reverse_iterator;
				typedef int& reference;
				typedef const int& const_reference;
				typedef cgv::type::size_type size_type;
				typedef int value_type;
				static size_type size() { return 4; }
				iterator begin() { return c; }
				const_iterator begin() const { return c; }
				iterator end() { return c+4; }
				const_iterator end() const { return c+4; }
				reverse_iterator rbegin() { return c; }
				const_reverse_iterator rbegin() const { return c; }
				reverse_iterator rend() { return c+4; }
				const_reverse_iterator rend() const { return c+4; }
				reference operator [] (size_type i) { return c[i]; }
				const_reference operator [] (size_type i) const { return c[i]; }
			};
		}
	}
}