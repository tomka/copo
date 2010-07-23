#pragma once

#include <cgv/range/ForwardRange.h>

namespace cgv {
	namespace range {
		namespace BidirectionalRange {
			/** a type is of the concept BidirectionalRange if it implements the concept ForwardRange, where the iterator
			    types are of concept BidirectionalIterator, if it defines the two types reverse_iterator and 
				const_reverse_iterator of concept BidirectionalIterator and if the following 
				expressions are valid and can be converted to the indicated type. 
				\code
				typedef ... reverse_iterator;
				typedef ... const_reverse_iterator;
				reverse_iterator(a.rbegin());
				reverse_iterator(a.rend());
				const_reverse_iterator((const T&)(a).rbegin());
				const_reverse_iterator((const T&)(a).rend());
				\endcode
				*/
			struct tag : public type::concepts<ForwardRange::tag> { };

			namespace Traits {
				/// not specialized implementation 
				template <typename T>
				struct traits : public cgv::type::not_specialized 
				{
					typedef typename cgv::type::cond::false_type implements;
				};
				/// implementation for tagged types
				template <typename T>
				struct traits<type::concept<tag,T> > : 
					public ForwardRange::traits<type::concept<ForwardRange::tag, T> >
				{
					typedef typename T::reverse_iterator reverse_iterator;
					typedef typename T::const_reverse_iterator const_reverse_iterator;
					/// the promote_const_reverse_iterator type yields the const_reverse_iterator for type T with const modifier and the reverse_iterator type otherwise
					typedef typename cgv::type::ctrl::if_<cgv::type::cond::is_const<T>,const_reverse_iterator,reverse_iterator>::type promote_const_reverse_iterator;
				};
			}
			/** allows to check if a type is a bidirectional range and what the iterator and size types are. In addition
				it inherits the functionality of cgv::range::BidirectionalRange::traits and defines the types 
				\c promote_const_iterator and \c promote_const_reverse_iterator, which are equal to 
				\c iterator / \c reverse_iterator if the template parameter has no \c const modifier and equal to 
				\c const_iterator / \c const_reverse_iterator if it has. */
			template <typename T>
			struct traits : public cgv::type::pick<Traits::traits, typename cgv::type::func::drop_ref<T>::type>::type {};

			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				typedef typename type::func::make_const<typename type::func::make_ref<T>::type>::type T_const;
				void constraints()
				{
					CGV_DEFINES_ASSERT_CONCEPT(cgv::range::ForwardRange, T);
					CGV_DEFINES_ASSERT_CONCEPT(cgv::range::BidirectionalIterator, traits<T>::iterator);
					CGV_DEFINES_ASSERT_CONCEPT(cgv::range::BidirectionalIterator, traits<T>::const_iterator);
					CGV_DEFINES_ASSERT_CONCEPT(cgv::range::BidirectionalIterator, traits<T>::reverse_iterator);
					CGV_DEFINES_ASSERT_CONCEPT(cgv::range::BidirectionalIterator, traits<T>::const_reverse_iterator);
					traits<T>::reverse_iterator iter;
					iter = a.rbegin();
					iter = a.rend();
					traits<T>::const_reverse_iterator citer;
					citer = c.rbegin();
					citer = c.rend();
				}
				T a;
				T_const c;
			};
			/** derive your concept implementation from this tag in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(cgv::range::BidirectionalRange, derived_class);
			};
			/// reference implementation of a BidirectionalRange
			class arche_type : public type::concepts<tag>, public check<arche_type>
			{
				int c[4];
			public:
				typedef int* iterator;
				typedef const int* const_iterator;
				typedef int* reverse_iterator;
				typedef const int* const_reverse_iterator;
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
			};
		}
	}
}