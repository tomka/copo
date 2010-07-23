#ifndef CGV_STORAGE_VIEW_QUADRATIC_VIEW_H
#define CGV_STORAGE_VIEW_QUADRATIC_VIEW_H

#include <cgv/storage/view/types.h>
#include <cgv/storage/view/block_view.h>

namespace cgv {
	namespace storage {
		namespace view {
			/** the base of the quadratic view provides the functionality that is specific to the quadratic view
			    compared to the rectangular view. */
			template <cgv::type::size_type ta_dimensionality>
			class quadratic_view_base
			{
			private:
				// type of an extent that is used to express the extent of the view
				typedef typename collection::array<size_type, ta_dimensionality> extent_type;
			protected:
				/// store the number of entries in each dimension
				size_type extent;
				/// cash the number of storage elements in each dimension
				extent_type number_elements;
				///
				void set_number_elements()
				{
					number_elements[0] = extent;
					for (size_type dimension = 1; dimension < ta_dimensionality; ++dimension)
						number_elements[dimension] = number_elements[dimension-1]*extent;
				}
			public:
				/// construct with the given extent in all directions
				quadratic_view_base() { }
				/// query the current extent
				const size_type& get_extent() const   { return extent; }
				/// query the current extent
				const size_type& get_extent(size_type dimension) const   { return extent; }
			};

			/** the quadratic view provides an indexing scheme to interpret a linear storage as a quadratic array. 
			    The class is parameterized over the dimensionality. A dimensionality of 2 results in a square, a 
				dimensionality of 3 in a cube and so one. The extent in each dimension is the same for the quadric
				view. It is specified in the constructor or with set_extent. If different extents are necessary,
				use the rectangular view. */
			template <cgv::type::size_type ta_dimensionality>
			class quadratic_view : public block_view<quadratic_view_base<ta_dimensionality>, ta_dimensionality>
			{
			public:
				/// return the total number of storage elements for a given extent
				static size_type compute_number_elements(size_type extent) { return pow(extent,ta_dimensionality); }
				/// construct with the given extent in all directions
				quadratic_view(size_type _extent = 0) { set_extent(_extent); }
				/// set a different extent
				void set_extent(size_type _extent) { extent = _extent; set_number_elements(); }
			};
		}
	}
}

#endif