#pragma once

#include <cgv/type/cond/is_ref.h>
#include <cgv/type/func/make_const.h>
#include <cgv/type/func/drop_ref.h>
#include <cgv/type/default_initializer.h>

namespace cgv {
	namespace utils {
		/** the pair class of cgv also allows to use references as the storage class types that are passed as template
		    arguments. */
		template <typename T1, typename T2>
		struct pair
		{
			// store a pair of values
			typedef pair<T1, T2> type;
			/// argument types used for construction. A reference is added to type if necessary
			typedef typename cgv::type::ctrl::if_<cgv::type::cond::is_ref<T1>,
				T1, typename cgv::type::func::make_const<
				typename cgv::type::func::make_ref<T1>::type>::type>::type argument_type_1;
			typedef typename cgv::type::ctrl::if_<cgv::type::cond::is_ref<T2>,
				T2, typename cgv::type::func::make_const<
					typename cgv::type::func::make_ref<T2>::type>::type>::type argument_type_2;
//			typedef typename cgv::type::func::make_const<typename cgv::type::func::make_ref<T1>::type>::type argument_type_1;
//			typedef typename cgv::type::func::make_const<typename cgv::type::func::make_ref<T2>::type>::type argument_type_2;
			/// extraction of value types
			typedef typename cgv::type::func::drop_ref<T1>::type value_type_1;
			typedef typename cgv::type::func::drop_ref<T2>::type value_type_2;
			/// the storage types describe how the elements are stored
			typedef T1 storage_type_1;
			typedef T2 storage_type_2;
/*		private:
			static argument_type_1 first_dummy()  { static value_type_1 dummy; return dummy; }
			static argument_type_2 second_dummy() { static value_type_2 dummy; return dummy; }*/
		public:
/*			/// default constructor uses static dummies for initialization
			pair() : first(first_dummy()), second(second_dummy()) {}
			/// construct from one element
			pair(argument_type_1 _first) : first(_first), second(second_dummy) {}*/
			/// construct from two elements
			pair(argument_type_1 _first = cgv::type::default_initializer<T1>::get(),
				 argument_type_2 _second= cgv::type::default_initializer<T2>::get()) 
				 :  first(_first), second(_second) {}
			/// construct from other pair, allows conversion with different const modifiers
			template <typename U1, typename U2>
			pair(const pair<U1,U2>& p) : first(p.first), second(p.second) {}
			/// storage
			storage_type_1 first;
			storage_type_2 second;
		};
	}
}

