#ifndef CGV_STORAGE_VIEW_TRIANGULAR_VIEW_H
#define CGV_STORAGE_VIEW_TRIANGULAR_VIEW_H

#include <cgv/algorithm/sum.h>
#include <cgv/storage/view/types.h>
#include <cgv/storage/view/cursor.h>
#include <cgv/range/sub.h>
#include <vector>

namespace cgv {
	namespace storage {
		namespace view {
			/** the triangular view provides an indexing scheme to interpret a linear storage as a triangular array. 
			    The class is parameterized over the dimensionality. A dimensionality of 2 results in a triangle, a 
				dimensionality of 3 in a tetrahedron and so one. The extent in each dimension is the same. Valid
				positions are those, where the sum of indices is less than the extent in one direction. */
			template <cgv::type::size_type ta_dimensionality>
			class triangular_view : public types<ta_dimensionality>
			{
			private:
				/// store the number of entries in each dimension
				size_type extent;
				/// store the total number of elements
				size_type number_elements;
			public:
				/// type of the cursor that is used to express a storage location
				typedef typename cursor<ta_dimensionality> cursor_type;
				/// the number of elements computes to prod_{i=1..dimensionality} (extent+i)/(i+1), a cache is built to speed up the computation and the case of dimensionality == 1 is implemented in a specialization of triangular_view below
				static size_type compute_cached_number_elements(size_type dimensionality, size_type extent)
				{
					// cache all values in a vector, where we ensure that all allocated cache values are computed
					static std::vector<std::vector<size_type> > number_elements_cache_array;
					// construct cache values in increasing dimension 
					for (size_type dimension = 1; dimension <= dimensionality; ++dimension) {
						// ensure that vector is allocated
						if (dimension >= number_elements_cache_array.size())
							number_elements_cache_array.push_back(std::vector<size_type>());
						// 
						std::vector<size_type>& number_elements_cache = number_elements_cache_array[dimension-1];
						// if the queried value has not been computed yet
						if (extent >= number_elements_cache.size()) {
							// store the index of the first to be computed cache entry
							size_type start_extent = (size_type) number_elements_cache.size();
							// resize the cache
							number_elements_cache.resize(extent+1);
							// compute the newly caches values
							if (dimension == 1) {
								for (size_type i = start_extent; i <= extent; ++i) {
									std::cout << "setting n0 (" << i << ") to " << i << std::endl;
									number_elements_cache[i] = i;
								}
							}
							else {
								// 
								std::vector<size_type>& number_elements_previous_cache = number_elements_cache_array[dimension-2];
								for (size_type i = start_extent; i <= extent; ++i) {
									std::cout << "setting n" << dimension << " (" << i << ") to " << ( number_elements_previous_cache[i] *
																 (i+dimension-1) ) / dimension  << std::endl;
									// just multiply and divide lower dimensional value
									number_elements_cache[i] = ( number_elements_previous_cache[i] *
																 (i+dimension-1) ) / dimension;
								}
							}
						}
					}
					// return cached value
					return number_elements_cache_array[dimensionality-1][extent];
				}
				/// the number of elements computes to prod_{i=1..dimensionality} (extent+i)/(i+1), a cache is built to speed up the computation 
				static size_type compute_number_elements(size_type extent)
				{
					return compute_cached_number_elements(dimensionality, extent);
				}
				/// compute the linear index of a given higher dimensional index
				index_type compute_index(const multi_index_type& multi_index) const
				{
					index_type n = extent;
					index_type l = 0;
					for (size_type dimension=dimensionality; dimension>1; --dimension) {
						l += compute_cached_number_elements(dimension,n);
						n -= multi_index[dimension-1];
						l -= compute_cached_number_elements(dimension,n);
					}
					l += multi_index[0];
					return l;
				}
				/// compute the higher dimensional index of a given linear index - current implementation is not very fast
				multi_index_type compute_multi_index(index_type index) const
				{
					multi_index_type multi_index = 0;
					index_type n = extent;
					for (size_type dimension = dimensionality; dimension > 0; ) {
						--dimension;
						index_type li;
						do {
							li = compute_index(multi_index);
							if (li == index)
								return multi_index;
							if (li < index) {
								if (n == 0)
									break;
								++multi_index[dimension];
								--n;
							}
							else {
								--multi_index[dimension];
								++n;
								break;
							}
						} while (li < index);
					}
					return multi_index;
				}
				/// correct the linear index of the cursor and return the delta in the linear index
				index_type correct_index(cursor_type& cursor) const
				{
					index_type index = compute_index(cursor.multi_index);
					index_type delta = index - cursor.index;
					cursor.index = index;
					return delta;
				}
				/// query the current extent
				const size_type& get_extent() const   { return extent; }
				/// construct with the given extent in all directions
				triangular_view(size_type _extent = 0) { set_extent(_extent); }
				/// set a different extent
				void set_extent(size_type _extent) { extent = _extent; number_elements = compute_number_elements(extent); }
				/// return the total number of storage elements
				size_type get_number_elements() const { return number_elements; }
				/// check if an index specifies a valid storage location
				bool is_valid_multi_index(const multi_index_type& multi_index) const 
				{
					for (size_type dimension=0; dimension<dimensionality; ++dimension) 
						if (multi_index[dimension] < 0)
							return false;
					return cgv::algorithm::sum(multi_index) < extent;
				}
				/// return a cursor that points to the first storage elements
				cursor_type get_first_element_cursor() const { return cursor_type(); }
				/// return a cursor that points to a specific storage element
				cursor_type get_indexed_element_cursor(const multi_index_type& multi_index) const 
				{ 
					return cursor_type(compute_index(multi_index),multi_index);
				}
				/// check if the cursor has reached the last storage element
				bool at_last(const cursor_type& cursor) const 
				{ 
					return cursor.index >= (index_type)get_number_elements()-1; 
				}
				/// assumption is that at_last(cursor) returns false, step to the next storage element in the order of the linear index
				void step(cursor_type& cursor) const
				{
					++cursor.index;
					size_type dimension = 0;
					index_type sum = cgv::algorithm::sum(cursor.multi_index);
					do {
						if (sum >= (index_type)extent-1) {
							sum -= (index_type)cursor.multi_index[dimension];
							cursor.multi_index[dimension] = 0;
						}
						else {
							++cursor.multi_index[dimension];
							return;
						}
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
					return cgv::algorithm::sum(cursor.multi_index) >= (index_type)get_extent()-1;
				}
				/// assumes that directional_at_last(cursor) returns false, step to next location in given dimension and return the distance jumped in the linear index
				index_type directional_step(cursor_type& cursor, size_type dimension) const
				{
					++cursor.multi_index[dimension];
					if (dimension == 0) {
						++cursor.index;
						return 1;
					}
					else
						return correct_index(cursor);
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
					return is_valid_multi_index(cursor.multi_index+multi_delta); 
				}
				/// assumes that is_valid_jump(cursor, multi_delta) returns true, adds the given multi_delta to the cursor location; careful this operation is not optimized and runs in O(abs_sum(multi_delta)) time
				index_type jump(cursor_type& cursor, const multi_index_type& multi_delta) const
				{
					cursor.multi_index += multi_delta;
					return correct_index(cursor);
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
				bool directional_is_valid_jump(const cursor_type& cursor, size_type dimension, index_type directional_delta) const 
				{
					multi_index_type target_index  = cursor.multi_index;
					target_index[dimension] += directional_delta;
					return is_valid(target_index);
				}
				/// assumes that is_valid_directional_jump(cursor, dimension, directional_delta) returns true, adds the given directional_delta to the cursor location; sorry this is neither optimized
				index_type directional_jump(cursor_type& cursor, size_type dimension, index_type directional_delta) const
				{
					cursor.multi_index[dimension] += directional_delta;
					return correct_index(cursor);
				}
				/// same as directional_jump(), but also update the given iterator according to the change of the linear index
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