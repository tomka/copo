#ifndef CGV_STORAGE_VIEW_BLOCK_VIEW_H
#define CGV_STORAGE_VIEW_BLOCK_VIEW_H

#include <cgv/storage/view/types.h>
#include <cgv/storage/view/cursor.h>

namespace cgv {
	namespace storage {
		namespace view {
			/** the block view is the base class of quadratic and rectangular view. */
			template <typename base_class, cgv::type::size_type ta_dimensionality>
			class block_view : public base_class, public types<ta_dimensionality>
			{
			public:
				/// type of an index that is used to express a storage location
				typedef typename cursor<ta_dimensionality> cursor_type;
				/// construct with the given extent in all directions
				block_view() { }

				/// return the total number of storage elements
				size_type get_number_elements() const { return number_elements[dimensionality-1]; }
				/// check if a multi index specifies a valid storage location
				bool is_valid_multi_index(const multi_index_type& multi_index) const 
				{
					for (unsigned int dimension=0; dimension<dimensionality; ++dimension) 
						if (multi_index[dimension] < 0 || multi_index[dimension] >= get_extent(dimension)) 
							return false; 
					return true; 
				}
				/// return a cursor that points to the first storage elements
				cursor_type get_first_element_cursor() const { return cursor_type(); }
				/// return a cursor that points to a specific storage element
				cursor_type get_indexed_element_cursor(const multi_index_type& multi_index) const 
				{ 
					cursor_type cursor(extent); 
					jump(cursor, multi_index); 
					return cursor;
				}
				/// check if the cursor has reached the last storage element
				bool at_last(const cursor_type& cursor) const { return cursor.index >= (index_type)get_number_elements()-1; }
				/// assumption is that at_last(cursor) returns false, step to the next storage element in the order of the linear index
				void step(cursor_type& cursor) const
				{
					++cursor.index;
					unsigned int dimension = 0;
					do {
						if (++cursor.multi_index[dimension] < (index_type)get_extent(dimension))
							return;
						cursor.multi_index[dimension] = 0;
						++dimension;
					} while (dimension < dimensionality);
				}
				/// same as step(), but also update the given iterator according to the change of the linear index
				template <typename iterator_type>
				void step(cursor_type& cursor, iterator_type& iterator) const
				{ 
					step(cursor); 
					++iterator; 
				}
				/// check if the cursor has reached the last storage element in the given dimension
				bool at_last(const cursor_type& cursor, size_type dimension) const
				{
					return cursor.index[dimension] >= (index_type)get_extent(dimension)-1;
				}
				/// assumes that at_last(cursor,dimension) returns false, step to next location in given dimension and return the distance jumped in the linear index
				index_type directional_step(cursor_type& cursor, size_type dimension) const
				{
					index_type delta = 1;
					if (dimension == 0)
						++cursor.index;
					else {
						delta = number_elements[dimension-1];
						cursor.index += delta;
					}
					++cursor.multi_index[dimension];
					return delta;
				}
				/// same as directional_step(), but also update the given iterator according to the change of the linear index
				template <typename iterator_type>
				index_type directional_step(cursor_type& cursor, size_type dimension, iterator_type& iterator) const
				{ 
					index_type delta = directional_step(cursor, dimension); 
					iterator += delta;
					return delta;
				}
				/// check if a jump by the given multi delta results in a valid storage location
				bool is_valid_jump(const cursor_type& cursor, const multi_index_type& multi_delta) const 
				{
					return is_valid_index(cursor.multi_index+multi_delta); 
				}
				/// assumes that is_valid_jump(cursor, multi_delta) returns true, adds the given multi delta to the cursor location
				index_type jump(cursor_type& cursor, const multi_index_type& multi_delta) const
				{
					index_type delta = multi_delta[0];
					cursor.multi_index[0] += delta;
					for (size_type dimension = 1; dimension < dimensionality; ++dimension) {
						delta += number_elements[dimension-1]*multi_delta[dimension];
						cursor.multi_index[dimension] += multi_delta[dimension];
					}
					cursor.index += delta;
					return delta;
				}
				/// same as jump(), but also update the given iterator according to the change of the linear index
				template <typename iterator_type>
				index_type jump(cursor_type& cursor, const multi_index_type& multi_delta, iterator_type& iterator) const
				{
					index_type delta = jump(cursor, multi_delta);
					iterator += delta;
					return delta;
				}
				/// check if a directional jump by the given directional_delta results in a valid storage location
				bool is_valid_directional_jump(const cursor_type& cursor, size_type dimension, index_type directional_delta) const 
				{
					index_type i = cursor.multi_index[dimension]+directional_delta;
					return (i >= 0 && i < get_extent(dimension));
				}
				/// assumes that is_valid_directional_jump(cursor, dimension, directional_delta) returns true, adds the given directional_delta to the cursor location
				index_type directional_jump(cursor_type& cursor, size_type dimension, index_type directional_delta) const
				{
					index_type delta = dimension == 0 ? directional_delta : number_elements[dimension-1]*directional_delta;
					cursor.index += delta;
					cursor.multi_index[dimension] += directional_delta;
					return delta;
				}
				/// same as directional_jump(), but also update the given iterator according to the change of the linear delta
				template <typename iterator_type>
				index_type directional_jump(cursor_type& cursor, size_type dimension, index_type directional_delta, iterator_type& iterator) const
				{
					index_type delta = directional_jump(cursor, dimension, directional_delta);
					iterator += delta;
					return delta;
				}
			};
		}
	}
}

#endif