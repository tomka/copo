#ifndef CGV_ALGORITHM_PRODUCT_H
#define CGV_ALGORITHM_PRODUCT_H

#include <algorithm>
#include <cgv/range/iterator_traits.h>
#include <cgv/type/drop_const_traits.h>

namespace cgv {
	namespace algorithm {
		/** the function object to determine the sum*/
		template <typename type>
		class product_computer
		{
		private:
			/// accumulate the sum here
			type product;
		public:
			/// initialize the sum to 0
			product_computer() { product = 1; }	
			// the function call to process the next elment
			void operator ( ) (type elem ) { product *= elem; }
			// return sum
			operator type ( ) { return product; }
		};

		/// iterator interface to summation
		template <typename iterator_type>
		typename cgv::range::iterator_value_traits<iterator_type>::type product(const iterator_type& begin, const iterator_type& end)
		{
			return for_each(begin,end,product_computer<typename cgv::type::drop_const_traits<iterator_value_type_traits<iterator_type>::type> >());
		}
		/// range interface to summation
		template <typename range_type>
		typename cgv::type::drop_const_traits<typename range_type::value_type>::type product(const range_type& range)
		{
			return for_each(range.begin(),range.end(),product_computer<typename cgv::type::drop_const_traits<typename range_type::value_type>::type>());
		}
	}
}

#endif