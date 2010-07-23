#pragma once

namespace cgv {
	namespace type {
		namespace ctrl {
			/// transform a template with one argument to a predicate
			template <template <typename _T> class S>
			struct pred
			{
				template <typename T>
				struct apply
				{
					typedef typename S<T>::type type;
				};
			};
			/// transform a template with two arguments to a predicate
			template <template <typename _T1, typename _T2> class S>
			struct pred2
			{
				template <typename T1, typename T2>
				struct apply
				{
					typedef typename S<T1,T2>::type type;
				};
			};
			/// transform a template with three arguments to a predicate
			template <template <typename _T1, typename _T2, typename _T3> class S>
			struct pred3
			{
				template <typename T1, typename T2, typename T3>
				struct apply
				{
					typedef typename S<T1,T2,T3>::type type;
				};
			};
			/// transform a condition template with one argument to a predicate
			template <template <typename _T> class S>
			struct cond_pred
			{
				template <typename T>
				struct apply
				{
					typedef S<T> type;
				};
			};
			/// transform a condition template with two arguments to a predicate
			template <template <typename _T1, typename _T2> class S>
			struct cond_pred2
			{
				template <typename T1, typename T2>
				struct apply
				{
					typedef S<T1,T2> type;
				};
			};
			/// transform a condition template with three arguments to a predicate
			template <template <typename _T1, typename _T2, typename _T3> class S>
			struct cond_pred3
			{
				template <typename T1, typename T2, typename T3>
				struct apply
				{
					typedef S<T1,T2,T3> type;
				};
			};
		}
	}
}
