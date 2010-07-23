#pragma once

#include <cgv/type/cond/is_base_of.h>
#include <cgv/type/cond/not.h>
#include <cgv/type/cond/and.h>
#include <cgv/type/cond/is_const.h>

namespace cgv {
	namespace type {
		namespace cast {
			/// \cond
			namespace detail {
				template <typename B, typename D, bool isbase>
				struct base_ptr_cast_impl
				{
					static B* cast(D* t) { return 0; }
				};
				template <typename B, typename D>
				struct base_ptr_cast_impl<B,D,true>
				{
					static B* cast(D* t) { return static_cast<B*>(t); }
				};
				template <typename B, typename D, bool isconst>
				struct base_ptr_cast
				{
					static B* cast(D* t)
					{
						return base_ptr_cast_impl<B,D,
							cond::and<
								cond::not<cond::is_const<D> >,
								cond::is_base_of_clean<B,D>
							>::value>::cast(t);
					}
				};
				template <typename B, typename D>
				struct base_ptr_cast<B,D,true>
				{
					static B* cast(D* t)
					{
						return base_ptr_cast_impl<B,D,
							cond::is_base_of_clean<B,D> >::cast(t);
					}
				};
			}
			/// \endcond
			/** the base_ptr cast converts a pointer to D to a pointer to B iff
				B is a base of D and either B is const or D is not const. If
				the cast fails, 0 is returned. See the cast_tests.cxx for samples. */
			template <typename B, typename D>
			B* base_ptr(D* t)
			{
				return detail::base_ptr_cast<B,D,cond::is_const<B>::value>::cast(t);
			}
		}
	}
}
