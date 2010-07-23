#pragma once
#pragma warning (disable:4996)

#include <algorithm>
#include <cgv/range/iterator_traits.h>
#include <cgv/range/operators.h>
#include <cgv/type/drop_const_traits.h>

namespace cgv {
	namespace algorithm {
		/** the function object to determine the sum*/
		template <typename iterator_type_1, typename iterator_type_2>
		class skp_computer
		{
		private:
			typedef typename cgv::type::drop_const_traits<typename cgv::range::iterator_value_traits<iterator_type_1>::type>::type type_1;
			typedef typename cgv::type::drop_const_traits<typename cgv::range::iterator_value_traits<iterator_type_2>::type>::type type_2;
			type_1 skp;
			iterator_type_2 iterator;
		public:
			/// initialize the sum to 0
			skp_computer(const iterator_type_2& _iterator) : iterator(_iterator) 
			{
				skp = 0; 
			}
			// the function call to process the next elment
			void operator () (type_1 elem)
			{
				elem *= *iterator;
				++iterator;
				skp += elem; 
			}
			// return sum
			operator type_1 () { return skp; }
		};

		/// iterator interface to skalar product, the return type is determined from the first iterator type
		template <typename iterator_type_1, typename iterator_type_2>
		typename cgv::range::iterator_value_traits<iterator_type_2>::type skp(const iterator_type_1& begin_1, const iterator_type_1& end_1, const iterator_type_2& begin_2)
		{
			return std::for_each(begin_1,end_1,skp_computer<iterator_type_1, iterator_type_2>(begin_2));
		}
		/// range interface to skalar product
		template <typename range_type_1, typename range_type_2>
		typename cgv::type::drop_const_traits<typename range_type_1::value_type>::type skp(const range_type_1& range_1, const range_type_2& range_2)
		{
			return skp(range_1.begin(),range_1.end(),range_2.begin());
		}
	}
}
