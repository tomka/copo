#pragma once

#include <cgv/type/reflect/realize.h>

namespace cgv {
	namespace type {
		namespace reflect {
			/// the call_function calls the function pointed to by the given function pointer and uses the passed realization of the function arguments as calling arguments. It returns the return value of the function. Functions with up to 8 arguments are supported.
			template <typename R, typename Real>
			R call_function(R (*fp)(), const Real&)
			{
				return (*fp)();
			}
			template <typename R, typename T1, typename Real>
			R call_function(R (*fp)(T1), const Real& r)
			{
				return (*fp)(r.ref<0>());
			}
			template <typename R, typename T1, typename T2, typename Real>
			R call_function(R (*fp)(T1,T2), const Real& r)
			{
				return (*fp)(r.ref<0>(),r.ref<1>());
			}
			template <typename R, typename T1, typename T2, typename T3, typename Real>
			R call_function(R (*fp)(T1,T2,T3), const Real& r)
			{
				return (*fp)(r.ref<0>(),r.ref<1>(),r.ref<2>());
			}
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename Real>
			R call_function(R (*fp)(T1,T2,T3,T4), const Real& r)
			{
				return (*fp)(r.ref<0>(),r.ref<1>(),r.ref<2>(),r.ref<3>());
			}
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename Real>
			R call_function(R (*fp)(T1,T2,T3,T4,T5), const Real& r)
			{
				return (*fp)(r.ref<0>(),r.ref<1>(),r.ref<2>(),r.ref<3>(),r.ref<4>());
			}
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename Real>
			R call_function(R (*fp)(T1,T2,T3,T4,T5,T6), const Real& r)
			{
				return (*fp)(r.ref<0>(),r.ref<1>(),r.ref<2>(),r.ref<3>(),r.ref<4>(),r.ref<5>());
			}
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename Real>
			R call_function(R (*fp)(T1,T2,T3,T4,T5,T6,T7), const Real& r)
			{
				return (*fp)(r.ref<0>(),r.ref<1>(),r.ref<2>(),r.ref<3>(),r.ref<4>(),r.ref<5>(),r.ref<6>());
			}
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename Real>
			R call_function(R (*fp)(T1,T2,T3,T4,T5,T6,T7,T8), const Real& r)
			{
				return (*fp)(r.ref<0>(),r.ref<1>(),r.ref<2>(),r.ref<3>(),r.ref<4>(),r.ref<5>(),r.ref<6>(),r.ref<7>());
			}
		}
	}
}
