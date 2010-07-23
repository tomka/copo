#pragma once

#include <cgv/utils/pair.h>
#include <cgv/type/concept.h>
#include <cgv/type/ds/list_alg.h>
#include <cgv/type/cond/is_less.h>
#include <cgv/type/cond/and.h>
#include <cgv/type/reflect/consts.h>
#include <cgv/type/reflect/realize.h>
#include <cgv/type/func/promote.h>

namespace cgv {
	namespace type {
		namespace expr {
			template <typename E1, typename E2, class DC> 
			struct binary_expression : 
				public concepts<Expression::tag>, Expression::check<binary_expression<E1,E2,DC> >,
				public cgv::utils::pair<
					typename Expression::traits<E1>::storage_type,
					typename Expression::traits<E2>::storage_type
				>
			{
				typedef binary_expression<E1,E2,DC> this_type;
				/// define type of pair base class
				typedef cgv::utils::pair<storage_type_1,storage_type_2> pair_base_type;
				/// unite placeholder sets of expressions E1 and E2
				typedef typename ds::unite<typename E1::placeholder_set, typename E2::placeholder_set>::type placeholder_set;
				/// 
				typedef const storage_type_1& construct_argument_type_1;
				typedef const storage_type_2& construct_argument_type_2;
				template <typename BI>
				struct binding_traits
				{
					typedef typename E1::binding_traits<BI> BT1;
					typedef typename E2::binding_traits<BI> BT2;
					/// expression is bound only if both E1 and E2 are bound
					typedef cond::and<typename BT1::is_bound, typename BT2::is_bound> is_bound;
					/// promote return type from return types of E1 and E2
					typedef typename ctrl::if_<is_bound, 
						typename func::promote<typename BT1::return_type,typename BT2::return_type>::type, 
						DC
					>::type return_type;
					typedef typename bind_struct<BI>::type BS;

/*					template <bool bound>
					static return_type select_execute(const this_type& ex, const BS& bs)
					{
						return BT1::execute(ex.first, bs), BT2::execute(ex.second, bs);
					}
					template <>
					static return_type select_execute<false>(const this_type& ex, const BS& bs)
					{
						return *this;
					}
					static return_type execute(const this_type& ex,  const BS& bs) 
					{
						return select_execute<is_bound::value>(ex, bs);
					}*/
				};
				/// construct from two expressions
				binary_expression(construct_argument_type_1 e1, construct_argument_type_2 e2) : 
					pair_base_type(e1,e2)
				{
				}
				/// define operator that allows to bind the first placeholders in E1 and E2
				template <typename T1>
				typename binding_traits<typename bind_info<DC,T1>::type>::return_type 
					operator() (const T1& v1) 
				{ 
					typedef typename bind_info<DC,T1>::type BI;
					typedef typename DC::binding_traits<BI> BT;
					typedef typename bind_struct<BI>::type BS;
					typedef typename select_placeholder<BI, typename ds::at<BI,0>::type::first>::type SP0;
					BS bs;
					SP0::ref(bs) = v1;
					return BT::execute(*static_cast<DC*>(this), bs);
				}
				/// define operator that allows to bind the first two placeholders in E1 and E2
				template <typename T1, typename T2>
				typename binding_traits<typename bind_info<DC,T1, T2>::type>::return_type 
					operator() (const T1& v1, const T2& v2) 
				{ 
					typedef typename bind_info<DC,T1,T2>::type BI;
					typedef typename DC::binding_traits<BI> BT;
					typedef typename bind_struct<BI>::type BS;
					const int pos1 = select_placeholder<BI, typename ds::at<BI,0>::type::first>::pos;
					const int pos2 = select_placeholder<BI, typename ds::at<BI,1>::type::first>::pos;
					BS bs;
					bs.ref<pos1>() = v1;
					bs.ref<pos2>() = v2;
					return BT::execute(*static_cast<DC*>(this), bs);
				}
				/// define operator that allows to bind the first three placeholders in E1 and E2
				template <typename T1, typename T2, typename T3>
				typename binding_traits<typename bind_info<DC,T1,T2,T3>::type>::return_type 
					operator() (const T1& v1, const T2& v2, const T3& v3) 
				{ 
					typedef typename bind_info<DC,T1,T2,T3>::type BI;
					typedef typename DC::binding_traits<BI> BT;
					typedef typename bind_struct<BI>::type BS;
					typedef typename select_placeholder<BI, typename ds::at<BI,0>::type::first>::type SP0;
					typedef typename select_placeholder<BI, typename ds::at<BI,1>::type::first>::type SP1;
					typedef typename select_placeholder<BI, typename ds::at<BI,2>::type::first>::type SP2;
					BS bs;
					SP0::ref(bs) = v1;
					SP1::ref(bs) = v2;
					SP2::ref(bs) = v3;
					return BT::execute(*static_cast<DC*>(this), bs);
				}
				/// define operator that allows to bind the first four placeholders in E1 and E2
				template <typename T1, typename T2, typename T3, typename T4>
				typename binding_traits<typename bind_info<DC,T1,T2,T3,T4>::type>::return_type 
					operator() (const T1& v1, const T2& v2, const T3& v3, const T4& v4) 
				{ 
					typedef typename bind_info<DC,T1,T2,T3,T4>::type BI;
					typedef typename DC::binding_traits<BI> BT;
					typedef typename bind_struct<BI>::type BS;
					typedef typename select_placeholder<BI, typename ds::at<BI,0>::type::first>::type SP0;
					typedef typename select_placeholder<BI, typename ds::at<BI,1>::type::first>::type SP1;
					typedef typename select_placeholder<BI, typename ds::at<BI,2>::type::first>::type SP2;
					typedef typename select_placeholder<BI, typename ds::at<BI,3>::type::first>::type SP3;
					BS bs;
					SP0::ref(bs) = v1;
					SP1::ref(bs) = v2;
					SP2::ref(bs) = v3;
					SP3::ref(bs) = v4;
					return BT::execute(*static_cast<DC*>(this), bs);
				}
			};
		}
	}
}