#pragma once

#include <algorithm>

namespace cgv {
	namespace range {
		/** provide equal operator for ranges */
		template <typename range_type_1, typename range_type_2>
		bool operator == (const range_type_1& range_1, const range_type_2& range_2)
		{
			return std::equal(range_1.begin(), range_1.end(), range_2.begin());
		}
		/** provide equal operator for ranges */
		template <typename range_type_1, typename range_type_2>
		range_type_1 operator + (const range_type_1& range_1, const range_type_2& range_2)
		{
			range_type_1 result;
			std::transform(range_1.begin(),range_1.end(),range_2.begin(),result.begin(),std::plus<range_type_1::value_type>());
			return result;
		}
		/** provide equal operator for ranges 
		template <typename range_type_1, typename range_type_2>
		range_type_1& operator += (range_type_1& range_1, const range_type_2& range_2)
		{
			std::transform(range_1.begin(),range_1.end(),range_2.begin(),range_1.begin(),std::plus<range_type_1::value_type>());
			return range_1;
		}
		*/
	}
}

#endif