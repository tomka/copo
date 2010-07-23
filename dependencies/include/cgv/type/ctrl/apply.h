#pragma once

namespace cgv {
	namespace type {
		namespace ctrl {
			/// applies the predicate P recursively N times to the type T, where N defaults to one 
			template <typename P, typename T, int N = 1> struct apply
			{ 
				typedef typename apply<P, typename P::apply<T>::type,N-1>::type type;
			};
			template <typename P, typename T> struct apply<P,T,0> { typedef T type; };
		}
	}
}
