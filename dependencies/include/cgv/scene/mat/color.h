#pragma once

#include <cgv/type/size_type.h>
#include <cgv/storage/collection/array.h>

namespace cgv {
	namespace scene {
		namespace mat {

/// enum with the different color models. The integer value modulo 10 determines the number of components
enum color_model { RGB = 3, RGBA = 4, HLS = 13, HLSA = 14, LUM = 1, LUMA = 2, YUV = 23, YUVA = 24, LUV = 33, LUVA = 34, LAB = 43, LABA = 44 };

template <typename T, color_model cm = RGB> class color;

/// template for conversion of one color[model] to another
template <typename T1, color_model cm1, typename T2, color_model cm2>
void convert_color(const color<T1,cm1>& from, color<T2,cm2>& to);

/** represent a color with the given number of components. If the
    number of components is not given, you need to define it in the
	constructor or with resize. */
template <typename T, color_model cm>
class color : public storage::collection::array<T,(type::uint32)cm%10>
{
public:
	const static color_model clr_mod = cm;
	color() { }
	color(const T& c0) { (*this)[0] = c0; }
	color(const T& c0, const T& c1) : array(c0,c1) {}
	color(const T& c0, const T& c1, const T& c2) : array(c0,c1,c2) {}
	color(const T& c0, const T& c1, const T& c2, const T& c3) : array(c0,c1,c2,c3) {}
	template <typename T2, color_model cm2>
	color(const color<T2,cm2>& c2)
	{
		convert_color(c2, *this);
	}
	/** assignment from 
		- constant   ... fill all elements with const value
		- range      ... copy values in range
		- expression ... set values in range to evaluated values in expression. If the expression is a range such es a(_i), follow the rules of assignment from ranges
	*/	
	template <typename _ta_type>
	typename cgv::range::expression::assign_expression<this_type&, _ta_type> operator = (const _ta_type& value)
	{
		storage::collection::Array::traits<T, (type::uint32)cm%10>::prepare<_ta_type>::assign(*this, value);
		return typename cgv::range::expression::assign_expression<this_type&, _ta_type>(*this, value);
	}
};

// standard implementation shows error message
template <typename T1, color_model cm1, typename T2, color_model cm2>
void convert_color(const color<T1,cm1>& c1, color<T2,cm2>& c2)
{
	std::cerr << "color conversion not implemented" << std::endl;
}

/// specialization for conversion from LUM to RGB
template <typename T1, typename T2>
void convert_color(const color<T1,LUM>& c1, color<T2,RGB>& c2)
{
	c2 = c1[0];
}

		}
	}
}