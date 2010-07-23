#ifndef CGV_STORAGE_VIEW_RECTANGULAR_VIEW_H
#define CGV_STORAGE_VIEW_RECTANGULAR_VIEW_H

#include <cgv/storage/view/types.h>
#include <cgv/storage/view/block_view.h>
#include <cgv/algorithm/product.h>

namespace cgv {
	namespace storage {
		namespace view {
			/** the base of the rectangular view provides the functionality that is specific to the rectangular view
			    compared to the quadratic view. */
			template <cgv::type::size_type ta_dimensionality>
			class rectangular_view_base
			{
			private:
				// type of an extent that is used to express the extent of the view
				typedef typename collection::array<size_type,ta_dimensionality> extent_type;
			protected:
				/// store the number of entries in each dimension
				extent_type extent;
				/// cash the number of storage elements in each dimension
				extent_type number_elements;
				///
				void set_number_elements()
				{
					number_elements[0] = extent[0];
					for (size_type dimension = 1; dimension < ta_dimensionality; ++dimension)
						number_elements[dimension] = number_elements[dimension-1]*extent[dimension];
				}
			public:
				/// construct with the given extent in all directions
				rectangular_view_base() { }
				/// query the current extent
				const extent_type& get_extent() const   { return extent; }
				/// query the current extent
				const size_type& get_extent(size_type dimension) const   { return extent[dimension]; }
			};

			/** the rectangular view provides an indexing scheme to interpret a linear storage as a rectangular array. 
			    The class is parameterized over the dimensionality. A dimensionality of 2 results in a rectangle, a 
				dimensionality of 3 in a cuboid and so one. The extent in each dimension is specified in the constructor 
				or with set_extent. If all extents are the same, use the quadratic view. */
			template <cgv::type::size_type ta_dimensionality>
			class rectangular_view : public block_view<rectangular_view_base<ta_dimensionality>, ta_dimensionality>
			{
			public:
				// type of an extent that is used to express the extent of the view
				typedef typename collection::array<size_type, ta_dimensionality> extent_type;
				/// return the total number of storage elements for a given extent
				static size_type compute_number_elements(const extent_type& extent) { return cgv::algorithm::product(extent); }
				/// construct with the given extent in all directions
				rectangular_view(const extent_type& _extent = extent_type(0)) { set_extent(_extent); }
				/// set a different extent
				void set_extent(const extent_type& _extent) { extent = _extent; set_number_elements(); }
				/// set a different extent in one dimension only
				void set_extent(size_type dimension, size_type _extent) { extent[dimension] = _extent; set_number_elements(); }
			};
		}
	}
}

#endif