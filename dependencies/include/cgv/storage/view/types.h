#pragma once
#include <stddef.h>
#include <cgv/type/size_type.h>
#include <cgv/type/index_type.h>
#include <cgv/storage/collection/array.h>

namespace cgv {
	namespace storage {
		namespace view {
			/** the quadratic base class declares the types common to the quadric view and the quadratic cursor. */
			template <cgv::type::size_type ta_dimensionality>
			struct types
			{
				/// size type
				typedef cgv::type::size_type size_type;
				/// index component
				typedef cgv::type::index_type index_type;
				/// save dimensionality in constant
				static const cgv::type::size_type dimensionality = ta_dimensionality;
				/// type of an index that is used to express a storage location
				typedef typename collection::array<index_type, ta_dimensionality> multi_index_type;
				/// type of an extent that is used to express the extent of the view
				typedef typename collection::array<size_type, ta_dimensionality> extent_type;
			};
		}
	}
}
