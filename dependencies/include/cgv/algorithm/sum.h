#ifndef CGV_ALGORITHM_SUM_H
#define CGV_ALGORITHM_SUM_H

#pragma warning (disable:4996)

#include <algorithm>
#include <cgv/range/iterator_traits.h>
#include <cgv/type/drop_const_traits.h>

namespace cgv {
	namespace algorithm {
		/** the function object to determine the sum*/
		template <typename type>
		class sum_computer
		{
		private:
			/// accumulate the sum here
			type sum;
		public:
			/// initialize the sum to 0
			sum_computer() { sum = 0; }	
			// the function call to process the next elment
			void operator ( ) (type elem ) { sum += elem; }
			// return sum
			operator type ( ) { return sum; }
		};

		/// iterator interface to summation
		template <typename iterator_type>
		typename cgv::range::iterator_value_traits<iterator_type>::type sum(const iterator_type& begin, const iterator_type& end)
		{
			return std::for_each(begin,end,sum_computer<typename cgv::type::drop_const_traits<iterator_value_traits<iterator_type>::type> >());
		}
		/// range interface to summation
		template <typename range_type>
		typename cgv::type::drop_const_traits<typename range_type::value_type>::type sum(const range_type& range)
		{
			return std::for_each(range.begin(),range.end(),sum_computer<typename cgv::type::drop_const_traits<typename range_type::value_type>::type>());
		}
	}
}

#endif