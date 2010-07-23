#pragma once 

#include <cgv/type/cond/bool.h>
#include <cgv/type/ctrl/pred.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/ctrl/foreach.h>

namespace cgv {
	namespace type {
		namespace cond {
			namespace And {
				template <typename T1, typename T2>
				struct binary_deref_and : public ctrl::if_<
					typename ds::deref<T1>::type,T2,cond::false_type>::type {};
			}
			/** combine up to 8 types which are derived from cond::true_type or cond::false_type with an \c and operator*/
			template <typename T1, typename T2, typename T3 = invalid_type, typename T4 = invalid_type, 
				      typename T5 = invalid_type, typename T6 = invalid_type, typename T7 = invalid_type, typename T8 = invalid_type>
			struct and : ctrl::foreach<typename ds::list<T1,T2,T3,T4,T5,T6,T7,T8>::type,
				ctrl::cond_pred2<And::binary_deref_and>,
				cond::true_type>::type {};
		}
	}
}
