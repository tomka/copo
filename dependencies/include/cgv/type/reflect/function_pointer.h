#pragma once

#include <cgv/type/ds/list.h>

namespace cgv {
	namespace type {
		namespace reflect {

/// defines for a given return type R and an argument type list L, a function pointer type. Only lists of up to length 8 are supported
template <typename R, typename L>
struct function_pointer
{
	template <int i> struct func_ptr { typedef invalid_type type; };
	template<> struct func_ptr<0> { typedef R (*type)(); };
	template<> struct func_ptr<1> { typedef R (*type)(typename ds::at<L,0>::type); };
	template<> struct func_ptr<2> { typedef R (*type)(typename ds::at<L,0>::type, typename ds::at<L,1>::type); };
	template<> struct func_ptr<3> { typedef R (*type)(typename ds::at<L,0>::type, typename ds::at<L,1>::type, typename ds::at<L,2>::type); };
	template<> struct func_ptr<4> { typedef R (*type)(typename ds::at<L,0>::type, typename ds::at<L,1>::type, typename ds::at<L,2>::type, typename ds::at<L,3>::type); };
	template<> struct func_ptr<5> { typedef R (*type)(typename ds::at<L,0>::type, typename ds::at<L,1>::type, typename ds::at<L,2>::type, typename ds::at<L,3>::type, typename ds::at<L,4>::type); };
	template<> struct func_ptr<6> { typedef R (*type)(typename ds::at<L,0>::type, typename ds::at<L,1>::type, typename ds::at<L,2>::type, typename ds::at<L,3>::type, typename ds::at<L,4>::type, typename ds::at<L,5>::type); };
	template<> struct func_ptr<7> { typedef R (*type)(typename ds::at<L,0>::type, typename ds::at<L,1>::type, typename ds::at<L,2>::type, typename ds::at<L,3>::type, typename ds::at<L,4>::type, typename ds::at<L,5>::type, typename ds::at<L,6>::type); };
	template<> struct func_ptr<8> { typedef R (*type)(typename ds::at<L,0>::type, typename ds::at<L,1>::type, typename ds::at<L,2>::type, typename ds::at<L,3>::type, typename ds::at<L,4>::type, typename ds::at<L,5>::type, typename ds::at<L,6>::type, typename ds::at<L,7>::type); };
	/// type of function pointer
	typedef typename func_ptr<ds::size<L>::value>::type type;
};

		}
	}
}