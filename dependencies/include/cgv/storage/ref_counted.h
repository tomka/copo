#pragma once

#include <cgv/type/cond/bool.h>
#include <cgv/type/concept.h>
#include <cgv/type/index_type.h>

namespace cgv {
	namespace storage {
		namespace RefCounted {
			/** the reference concept implies the existence of the following methods:
			
				index_type get_ref_count() const;
				void set_ref_count(index_type);

				furthermore, get_ref_count() must return 0 after construction.
			*/
			struct tag {};
			namespace Traits {
				template <typename T>
				struct traits : public cgv::type::not_specialized
				{
					typedef cgv::type::cond::false_type implements;
				};
				template <typename X>
				struct traits<cgv::type::concept<tag,X> >
				{
					typedef cgv::type::cond::true_type implements;
				};
			}
			/// the traits in namespace Traits define all marked types to implement the concept
			template <typename T>
			struct traits : public cgv::type::pick<Traits::traits,T>::type
			{
			};
			/// constraints necessary for concept checking
			template <typename T>
			struct concept_check 
			{
				T instance;
				void constraints()
				{
					instance.set_ref_count(instance.get_ref_count()+1);
				}
			};
			/** derive your concept implementation from this tag in order to automatically check the concept at compile-time */
			template <class derived_class>
			struct check
			{
				CGV_DEFINES_ASSERT_CONCEPT(cgv::storage::RefCounted, derived_class);
			};
		}

		/** standard implementation of a base class for a reference counted class */
		class ref_counted : public cgv::type::concepts<RefCounted::tag>, RefCounted::check<ref_counted>
		{
		private:
			cgv::type::index_type ref_count;
		public:
			ref_counted() : ref_count(0) {}
			cgv::type::index_type get_ref_count() const { return ref_count; }
			void set_ref_count(cgv::type::index_type c) { ref_count = c; }
		};
	}
}