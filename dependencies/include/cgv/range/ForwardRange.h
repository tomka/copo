#pragma once

#include <cgv/defines/assert.h>
#include <cgv/type/size_type.h>
#include <cgv/type/concept.h>
#include <cgv/type/Dereferencible.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/cond/is_const.h>
#include <cgv/type/cond/bool.h>
#include <cgv/type/func/clean.h>
#include <cgv/type/func/make_const.h>
#include <cgv/type/func/make_ref.h>

namespace cgv {
	namespace range {
		namespace ForwardRange {
			/** a type is of the concept ForwardRange if it defines two types iterator and const_iterator of concept 
			    ForwardIterator, a type size_type, a value_type and if the following expressions are valid and can be converted to 
				the indicated type:
				\code
				typedef ... iterator;
				typedef ... const_iterator;
				typedef ... size_type;
				typedef ... value_type;
				iterator(a.begin());
				iterator(a.end());
				const_iterator((const T&)(a).begin());
				const_iterator((const T&)(a).end());
				size_type(a.size());
				\endcode
				*/
			struct tag {};
			namespace Traits {
				/// not specialized implementation 
				template <typename T>
				struct traits : public cgv::type::not_specialized 
				{
					typedef typename cgv::type::cond::false_type implements;
				};
				/// implementation of traits for tagged forward ranges
				template <typename T>
				struct traits<type::concept<tag,T> >
				{
					typedef type::cond::true_type implements;
					typedef typename T::size_type size_type;
					typedef typename T::iterator iterator;
					typedef typename T::const_iterator const_iterator;
					typedef typename T::value_type value_type;
					typedef typename cgv::type::Dereferencible::traits<iterator>::reference reference;
					typedef typename cgv::type::Dereferencible::traits<const_iterator>::reference const_reference;
					/// the promote_const_iterator type yields the const_iterator for type T with const modifier and the iterator type otherwise
					typedef typename cgv::type::ctrl::if_<cgv::type::cond::is_const<T>,const_iterator,iterator>::type promote_const_iterator;
					/// the promote_const_reference type yields the const_reference for type T with const modifier and the reference type otherwise
					typedef typename cgv::type::ctrl::if_<cgv::type::cond::is_const<T>,
						typename cgv::type::Dereferencible::traits<const_iterator>::reference,
						typename cgv::type::Dereferencible::traits<iterator>::reference>::type promote_const_reference;
				};
			}
			/// allows to check if a type is a forward iterator and what the reference type is
			template <typename T>
			struct traits : public cgv::type::pick<Traits::traits, typename cgv::type::func::drop_ref<T>::type>::type {};
			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				typedef typename type::func::make_const<typename type::func::make_ref<T>::type>::type T_const;
				void constraints()
				{
					CGV_DEFINES_ASSERT_CONCEPT(ForwardIterator, typename traits<T>::iterator);
					CGV_DEFINES_ASSERT_CONCEPT(ForwardIterator, typename traits<T>::const_iterator);
					typename traits<T>::size_type size;
					size = a.size();
					typename traits<T>::iterator iter;
					iter = a.begin();
					v = *iter;
					iter = a.end();
					typename traits<T>::const_iterator citer;
					citer = c.begin();
					citer = c.end();
				}
				T a;
				T_const c;
				typename traits<T>::value_type v;
			};
			/** derive your concept implementation from this tag in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(ForwardRange, derived_class);
			};
			/// reference implementation of a ForwardRange
			class arche_type : public type::concepts<tag>, public check<arche_type>
			{
				int c[4];
			public:
				typedef int* iterator;
				typedef const int* const_iterator;
				typedef type::size_type size_type;
				typedef int value_type;
				static size_type size() { return 4; }
				iterator begin() { return c; }
				const_iterator begin() const { return c; }
				iterator end() { return c+4; }
				const_iterator end() const { return c+4; }
			};
		}
	}
}