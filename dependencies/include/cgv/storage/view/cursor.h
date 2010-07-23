#ifndef CGV_STORAGE_VIEW_CURSOR_H
#define CGV_STORAGE_VIEW_CURSOR_H

#include <cgv/storage/view/types.h>

namespace cgv {
	namespace storage {
		namespace view {
			/** a cursor keeps a linear and a multi-dimensional index. The views use the cursor to navigate 
			    storage space. */
			template <cgv::type::size_type ta_dimensionality>
			struct cursor : public types<ta_dimensionality>
			{
				/// store the current linear index
				index_type index;
				/// and the current multi dimensional index
				multi_index_type multi_index;
				/// construct cursor that points to the first storage element
				cursor() : index(0)
				{ 
					multi_index = 0;
				}
				/// construct cursor from linear index and multi dimensional index
				cursor(index_type l, const multi_index_type& i) : linear_index(l), index(i)
				{ 
				}
				/// equal operator 
				bool operator == (const cursor<ta_dimensionality>& c) const
				{
					return c.index == index && c.multi_index == multi_index;
				}
				/// print out to stream
				friend std::ostream& operator << (std::ostream& os, const cursor<dimensionality>& c)
				{
					return os << c.multi_index << "|" << c.index;
				}
			};
		}
	}
}

#endif