#pragma once

#include <cgv/type/ds/list.h>
#include <cgv/type/ctrl/apply.h>

namespace cgv {
	namespace type {
		namespace ctrl {
			namespace Foreach {
				template <typename I1, typename I2, typename P, typename T> 
				struct foreach
				{ 
					typedef typename foreach<typename cgv::type::ds::next<I1>::type, I2, P, 
						typename P::apply<I1, T>::type>::type type;
				};
				template <typename I, typename P, typename T> 
				struct foreach<I,I,P,T> 
				{
					typedef T type; 
				};

				template <typename I1, typename I2, typename P, typename T> 
				struct foreach_reverse
				{ 
					typedef typename P::apply<I1, 
						typename foreach_reverse<typename cgv::type::ds::next<I1>::type, I2, P, T>::type
					>::type type;
				};
				template <typename I, typename P, typename T> 
				struct foreach_reverse<I,I,P,T> 
				{
					typedef T type; 
				};
			}
			/// apply for each iterator in R the binary predicate P recursively to T by invokation of T = P::apply<I,T>
			template <typename R, typename P, typename T> struct foreach
			{ 
				typedef typename Foreach::foreach<typename ds::begin<R>::type, typename ds::end<R>::type, P, T>::type type;
			};
			/// apply for each iterator in R the binary predicate P by invokation of P::apply<I,T> in the reverse order of the range R
			template <typename R, typename P, typename T> struct foreach_reverse
			{ 
				typedef typename Foreach::foreach_reverse<typename ds::begin<R>::type, typename ds::end<R>::type, P, T>::type type;
			};
		}
	}
}
