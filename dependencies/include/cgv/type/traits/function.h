#pragma once

#include <cgv/type/ds/list.h>

namespace cgv {
	namespace type {
		namespace traits {
			/** defines for a function pointer type the return type and the
			    list of argument types. Only functions up to a maximum number of
				eight arguments are supported. */
			template <typename T> struct function
			{
				typedef invalid_type return_type;
				typedef invalid_type argument_list;				
			};
			template <typename R> 
			struct function<R (*)()>			
			{
				typedef R return_type;
				typedef ds::list<>::type argument_list;
			};
			template <typename R, typename T1> 
			struct function<R (*)(T1)>			
			{
				typedef R return_type;
				typedef typename ds::list<T1>::type argument_list;
			};
			template <typename R, typename T1, typename T2> 
			struct function<R (*)(T1, T2)>			
			{
				typedef R return_type;
				typedef typename ds::list<T1,T2>::type argument_list;
			};
			template <typename R, typename T1, typename T2, typename T3>
			struct function<R (*)(T1, T2, T3)>
			{
				typedef R return_type;
				typedef typename ds::list<T1,T2,T3>::type argument_list;
			};
			template <typename R, typename T1, typename T2, typename T3, typename T4>
			struct function<R (*)(T1, T2, T3, T4)>
			{
				typedef R return_type;
				typedef typename ds::list<T1,T2,T3,T4>::type argument_list;
			};
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
			struct function<R (*)(T1, T2, T3, T4, T5)>
			{
				typedef R return_type;
				typedef typename ds::list<T1,T2,T3,T4,T5>::type argument_list;
			};
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
			struct function<R (*)(T1, T2, T3, T4, T5, T6)>
			{
				typedef R return_type;
				typedef typename ds::list<T1,T2,T3,T4,T5,T6>::type argument_list;
			};
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
			struct function<R (*)(T1, T2, T3, T4, T5, T6, T7)>
			{
				typedef R return_type;
				typedef typename ds::list<T1,T2,T3,T4,T5,T6,T7>::type argument_list;
			};
			template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
			struct function<R (*)(T1, T2, T3, T4, T5, T6, T7, T8)>
			{
				typedef R return_type;
				typedef typename ds::list<T1,T2,T3,T4,T5,T6,T7,T8>::type argument_list;
			};
		}
	}
}