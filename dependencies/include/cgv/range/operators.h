#pragma once

#include <algorithm>

namespace cgv {
	namespace range {
		namespace ForwardRange {
			namespace Operator
			{
				// used to avoid problem with std::string interpretation as range
				template <typename T>
				inline void output(std::ostream& os, const T& v)
				{
					os << v;
				}
				template <>
				inline void output(std::ostream& os, const std::string& v)
				{
					std::operator << (os, v.c_str());
				}
			}
			/// output to stream
			template <typename range_type>
			std::ostream& operator << (std::ostream& os, const range_type& range)
			{

				std::copy(range.begin(), range.end()-1, std::ostream_iterator<range_type::value_type>(os,","));
				Operator::output(os, *(range.end()-1));
				return os;
			}
			/** provide equal operator for ranges */
			template <typename range_type>
			bool operator == (const range_type& range_1, const range_type& range_2)
			{
				return std::equal(range_1.begin(), range_1.end(), range_2.begin());
			}
			/** provide plus operator for ranges that are standard constructible */
			template <typename range_type>
			range_type operator + (const range_type& range_1, const range_type& range_2)
			{
				range_type_1 result;
				std::transform(range_1.begin(),range_1.end(),range_2.begin(),result.begin(),std::plus<range_type_1::value_type>());
				return result;
			}
			/** provide plus assign operator for ranges */
			template <typename range_type>
			range_type& operator += (range_type& range_1, const range_type& range_2)
			{
				std::transform(range_1.begin(),range_1.end(),range_2.begin(),range_1.begin(),std::plus<range_type::value_type>());
				return range_1;
			}
			/** provide minus operator for ranges that are standard constructible */
			template <typename range_type>
			range_type operator - (const range_type& range_1, const range_type& range_2)
			{
				range_type_1 result;
				std::transform(range_1.begin(),range_1.end(),range_2.begin(),result.begin(),std::minus<range_type_1::value_type>());
				return result;
			}
			/** provide minus assign operator for ranges */
			template <typename range_type>
			range_type& operator -= (range_type& range_1, const range_type& range_2)
			{
				std::transform(range_1.begin(),range_1.end(),range_2.begin(),range_1.begin(),std::minus<range_type::value_type>());
				return range_1;
			}
			/** provide divide assign operator for ranges */
			template <typename range_type>
			range_type& operator /= (range_type& range_1, const range_type& range_2)
			{
				std::transform(range_1.begin(),range_1.end(),range_2.begin(),range_1.begin(),std::divides<range_type::value_type>());
				return range_1;
			}
			/** provide divide assign operator for ranges */
			template <typename range_type>
			range_type& operator /= (range_type& range, const typename range_type::value_type& value)
			{
				struct divide_value
				{
				   const typename range_type::value_type& value;
				   divide_value(const typename range_type::value_type& _value) : value(_value) {}
				   void operator ( ) (typename range_type::value_type& element) const { element /= values; }
				} divider(value);
				std::for_each(range.begin(),range.end(),divider);
				return range;
			}
			/** provide multiply assign operator for ranges */
			template <typename range_type>
			range_type& operator *= (range_type& range_1, const range_type& range_2)
			{
				std::transform(range_1.begin(),range_1.end(),range_2.begin(),range_1.begin(),std::multiplies<range_type::value_type>());
				return range_1;
			}
			/** provide multiply assign operator for ranges */
			template <typename range_type>
			range_type& operator *= (range_type& range, const typename range_type::value_type& value)
			{
				struct multiply_value
				{
				   const typename range_type::value_type& value;
				   multiply_value(const typename range_type::value_type& _value) : value(_value) {}
				   void operator ( ) (typename range_type::value_type& element) const { element *= value; }
				} multiplier(value);
				std::for_each(range.begin(),range.end(),multiplier);
				return range;
			}
		}
	}
}
